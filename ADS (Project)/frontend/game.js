// ============================================================
// EscapeX - Frontend Game Logic
// 
// All communication with the C++ backend happens through
// fetch() calls to the API endpoints.
// No frameworks, no libraries — just vanilla JavaScript.
// ============================================================

// ---- GAME STATE (frontend) ----
var currentRoomId = -1;      // Which room the player is currently viewing
var timerInterval = null;    // Reference to the timer polling interval
var gameIsActive = false;    // Is the game currently running

// ============================================================
// SCREEN MANAGEMENT
// Show one screen, hide the others
// ============================================================

function showScreen(screenId) {
    // Hide all screens
    document.getElementById("start-screen").style.display = "none";
    document.getElementById("game-screen").style.display = "none";
    document.getElementById("result-screen").style.display = "none";

    // Show the requested screen
    document.getElementById(screenId).style.display = "flex";
}

// ============================================================
// START GAME
// Called when the player clicks "Start Game"
// ============================================================

async function startGame() {
    // Get the chosen timer duration from the dropdown
    var timerValue = document.getElementById("timer-select").value;

    // Call the backend API to start a new game
    var response = await fetch("/api/start?time=" + timerValue);
    var data = await response.json();

    // Switch to the game screen
    showScreen("game-screen");
    gameIsActive = true;
    currentRoomId = -1;

    // Update the room map with initial state
    updateRoomMap(data.rooms);

    // Update the score display
    updateScore(data.totalScore);

    // Show the default puzzle message
    showPuzzleDefault();

    // Start polling the server every second for timer updates
    startTimerPolling();
}

// ============================================================
// TIMER POLLING
// Every second, fetch the game state and update the timer
// ============================================================

function startTimerPolling() {
    // Clear any existing interval
    if (timerInterval !== null) {
        clearInterval(timerInterval);
    }

    // Poll every second
    timerInterval = setInterval(async function() {
        var response = await fetch("/api/state");
        var data = await response.json();

        // Update the timer display
        updateTimerDisplay(data.timeRemaining);

        // Update score (might change from background events)
        updateScore(data.totalScore);

        // Check if the game is over
        if (!data.gameActive) {
            gameIsActive = false;
            clearInterval(timerInterval);
            timerInterval = null;
            showResults();
        }
    }, 1000);
}

// Format seconds into M:SS and update the timer element
function updateTimerDisplay(seconds) {
    var minutes = Math.floor(seconds / 60);
    var secs = seconds % 60;
    var display = "\u23F1 " + minutes + ":" + (secs < 10 ? "0" : "") + secs;

    var timerElement = document.getElementById("timer");
    timerElement.textContent = display;

    // Change color based on remaining time
    timerElement.className = "";
    if (seconds <= 10) {
        timerElement.className = "danger";
    } else if (seconds <= 30) {
        timerElement.className = "warning";
    }
}

// Update the score display
function updateScore(score) {
    document.getElementById("score").textContent = "\uD83C\uDFC6 Score: " + score;
}

// ============================================================
// ROOM MAP
// Update the visual state of all 9 rooms
// ============================================================

function updateRoomMap(rooms) {
    for (var i = 0; i < rooms.length; i++) {
        var room = rooms[i];
        var element = document.getElementById("room-" + room.id);

        if (element === null) continue;

        // Remove all state classes
        element.className = "room";

        // Apply the correct state class
        if (room.isCompleted) {
            element.classList.add("completed");
        } else if (room.isLocked) {
            element.classList.add("locked");
        } else {
            element.classList.add("unlocked");
        }

        // Highlight the current room
        if (room.id === currentRoomId) {
            element.classList.add("current");
        }
    }
}

// Refresh the room map from the server
async function refreshRoomMap() {
    var response = await fetch("/api/state");
    var data = await response.json();
    updateRoomMap(data.rooms);
    updateScore(data.totalScore);
}

// ============================================================
// ENTER A ROOM
// Called when the player clicks on a room in the map
// ============================================================

async function enterRoom(roomId) {
    // Don't do anything if the game is over
    if (!gameIsActive) return;

    // Fetch room details from the backend
    var response = await fetch("/api/room/" + roomId);
    var data = await response.json();

    // Check for errors (room locked, etc.)
    if (data.error) {
        // Show a brief message for locked rooms
        if (data.error === "Room is locked") {
            showFeedback("This room is locked. Solve adjacent rooms first!", "error");
        }
        return;
    }

    // Set the current room
    currentRoomId = roomId;

    // If the room is already completed, show completed message
    if (data.isCompleted) {
        showPuzzleCompleted(data.name, data.score);
        refreshRoomMap();
        return;
    }

    // Show the puzzle for this room
    showPuzzle(data);

    // Refresh the room map to highlight the current room
    refreshRoomMap();
}

// ============================================================
// PUZZLE DISPLAY
// Show/hide the different puzzle area states
// ============================================================

function showPuzzleDefault() {
    document.getElementById("puzzle-default").style.display = "flex";
    document.getElementById("puzzle-content").style.display = "none";
    document.getElementById("puzzle-completed").style.display = "none";
}

function showPuzzle(data) {
    document.getElementById("puzzle-default").style.display = "none";
    document.getElementById("puzzle-content").style.display = "block";
    document.getElementById("puzzle-completed").style.display = "none";

    // Set room name and points
    document.getElementById("puzzle-room-name").textContent =
        "Room " + data.roomId + " - " + data.name;
    document.getElementById("puzzle-points").textContent = data.points + " pts";

    // Set the question text
    document.getElementById("question-text").textContent = data.question;

    // Build the answer options
    var optionsDiv = document.getElementById("options");
    optionsDiv.innerHTML = "";

    var labels = ["A", "B", "C", "D"];
    for (var i = 0; i < data.options.length; i++) {
        var btn = document.createElement("button");
        btn.className = "option-btn";
        btn.innerHTML = '<span class="option-label">' + labels[i] + '</span>' + data.options[i];
        btn.setAttribute("data-index", i);
        btn.onclick = function() {
            var index = parseInt(this.getAttribute("data-index"));
            submitAnswer(index);
        };
        optionsDiv.appendChild(btn);
    }

    // Reset hint
    var hintBtn = document.getElementById("hint-btn");
    hintBtn.disabled = false;
    if (data.hintUsed) {
        hintBtn.disabled = true;
    }
    document.getElementById("hint-text").style.display = "none";

    // Hide any previous feedback
    document.getElementById("feedback").style.display = "none";
}

function showPuzzleCompleted(roomName, score) {
    document.getElementById("puzzle-default").style.display = "none";
    document.getElementById("puzzle-content").style.display = "none";
    document.getElementById("puzzle-completed").style.display = "block";

    document.getElementById("completed-msg").textContent =
        "You already solved " + roomName + " and earned " + score + " points.";
}

// ============================================================
// SUBMIT ANSWER
// Called when the player clicks an answer option
// ============================================================

async function submitAnswer(optionIndex) {
    if (!gameIsActive || currentRoomId === -1) return;

    // Disable all option buttons to prevent double-click
    var buttons = document.querySelectorAll(".option-btn");
    for (var i = 0; i < buttons.length; i++) {
        buttons[i].disabled = true;
    }

    // Send the answer to the backend
    var response = await fetch("/api/answer", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ roomId: currentRoomId, answer: optionIndex })
    });
    var data = await response.json();

    if (data.correct) {
        // ---- CORRECT ANSWER ----

        // Highlight the correct option green
        buttons[optionIndex].classList.add("correct");

        // Show success feedback
        var msg = data.message;
        if (data.bonus) {
            msg += " +50 completion bonus!";
        }
        showFeedback(msg, "success");

        // Update the score
        updateScore(data.totalScore);

        // Update the timer
        updateTimerDisplay(data.timeRemaining);

        // After a short delay, refresh the map and show completed state
        setTimeout(function() {
            refreshRoomMap();
            showPuzzleCompleted(
                document.getElementById("puzzle-room-name").textContent,
                data.pointsEarned
            );
        }, 1200);

    } else {
        // ---- WRONG ANSWER ----

        // Highlight the wrong option red
        buttons[optionIndex].classList.add("wrong");

        // Show error feedback
        showFeedback(data.message, "error");

        // Update the timer
        updateTimerDisplay(data.timeRemaining);

        // Re-enable buttons after a short delay so the player can retry
        setTimeout(function() {
            for (var i = 0; i < buttons.length; i++) {
                buttons[i].disabled = false;
                buttons[i].classList.remove("wrong");
            }
        }, 800);
    }
}

// ============================================================
// SHOW HINT
// Called when the player clicks the hint button
// ============================================================

async function showHint() {
    if (currentRoomId === -1) return;

    // Fetch the hint from the backend
    var response = await fetch("/api/hint/" + currentRoomId);
    var data = await response.json();

    if (data.hint) {
        // Display the hint text
        var hintText = document.getElementById("hint-text");
        hintText.textContent = "\uD83D\uDCA1 " + data.hint;
        hintText.style.display = "block";

        // Disable the hint button (can only use once)
        document.getElementById("hint-btn").disabled = true;

        // Update the points display (reduced by 5)
        var pointsEl = document.getElementById("puzzle-points");
        var currentPoints = parseInt(pointsEl.textContent);
        if (!isNaN(currentPoints)) {
            pointsEl.textContent = Math.max(0, currentPoints - 5) + " pts";
        }
    }
}

// ============================================================
// FEEDBACK MESSAGES
// Show success/error messages in the puzzle area
// ============================================================

function showFeedback(message, type) {
    var feedbackEl = document.getElementById("feedback");
    feedbackEl.textContent = message;
    feedbackEl.className = type;
    feedbackEl.style.display = "block";

    // Auto-hide after a few seconds (for errors)
    if (type === "error") {
        setTimeout(function() {
            feedbackEl.style.display = "none";
        }, 2500);
    }
}

// ============================================================
// RESULTS SCREEN
// Called when the game ends (timer reaches 0)
// ============================================================

async function showResults() {
    // Fetch the completion history from the backend (from the Stack)
    var response = await fetch("/api/history");
    var data = await response.json();

    // Set the final score
    document.getElementById("final-score").textContent = data.totalScore;

    // Set rooms completed count
    var roomCount = data.history ? data.history.length : 0;
    document.getElementById("final-rooms").textContent = roomCount + " / 9";

    // Set time spent
    var timeSpent = data.timeSpent || 0;
    var minutes = Math.floor(timeSpent / 60);
    var secs = timeSpent % 60;
    document.getElementById("final-time").textContent =
        minutes + ":" + (secs < 10 ? "0" : "") + secs;

    // Build the history list (from Stack — LIFO order, most recent first)
    var historyDiv = document.getElementById("history-list");
    historyDiv.innerHTML = "";

    if (data.history && data.history.length > 0) {
        for (var i = 0; i < data.history.length; i++) {
            var item = data.history[i];
            var div = document.createElement("div");
            div.className = "history-item";
            div.innerHTML =
                '<span class="history-order">#' + (i + 1) + '</span>' +
                '<span class="history-name">Room ' + item.roomId + ' - ' + item.name + '</span>' +
                '<span class="history-score">+' + item.score + '</span>';
            historyDiv.appendChild(div);
        }
    } else {
        historyDiv.innerHTML = '<p class="history-empty">No rooms completed.</p>';
    }

    // Switch to the result screen
    showScreen("result-screen");
}
