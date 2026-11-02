// ============================================================
// EscapeX - Escape Room Puzzle Game Server
// 
// A complete game demonstrating 5 data structures:
//   1. Graph (Adjacency List) - Room connections
//   2. Stack               - Completed rooms history
//   3. Queue               - Question shuffling and distribution
//   4. HashMap             - Room data storage (O(1) lookup)
//   5. Priority Queue      - Dynamic difficulty management
//
// Compile (Windows): g++ -std=c++17 -o game_server.exe game_server.cpp -lws2_32
// Compile (Linux):   g++ -std=c++17 -o game_server game_server.cpp -lpthread
// Run:               ./game_server
// Play:              http://localhost:8080
// ============================================================

#include "httplib.h"
#include "questions.h"

#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <sstream>
#include <functional>

using namespace std;

// ============================================================
// STRUCTS — Simple data holders, no classes
// ============================================================

// Represents a single question assigned to a room
struct Question {
    int id;                    // Unique question ID
    string text;               // The question text
    string hint;               // A helpful hint
    vector<string> options;    // 4 answer choices
    int correctOption;         // Index of correct answer (0-3)
    int difficulty;            // 1=easy, 2=medium, 3=hard
    int points;                // 10, 20, or 30

    // Comparison operator for priority queue
    // Lower difficulty = lower priority (so higher difficulty comes out first)
    bool operator<(const Question& other) const {
        return difficulty < other.difficulty;
    }
};

// Represents a single room in the game
struct Room {
    int id;                    // Room ID (1-9)
    string name;               // Room name (e.g., "The Lobby")
    int difficulty;            // 1=easy, 2=medium, 3=hard
    bool isLocked;             // Can the player enter this room?
    bool isCompleted;          // Has the player solved this room?
    int questionId;            // ID of the assigned question (-1 if none)
    int score;                 // Points earned from this room
};

// ============================================================
// GLOBAL GAME STATE
// ============================================================

// --- DATA STRUCTURE 1: GRAPH (Adjacency List) ---
// Represents room connections. Index = room ID, value = list of connected room IDs.
// Room 0 is unused (rooms are numbered 1-9).
vector<vector<int>> adjacencyList(10);

// --- DATA STRUCTURE 2: STACK (Completed Rooms) ---
// Tracks the order of room completions. Push when completed, pop for history.
stack<int> completedStack;

// --- DATA STRUCTURE 3: QUEUE (Question Pool) ---
// Holds shuffled questions. Dequeue one for each room at game start.
queue<Question> questionQueue;

// --- DATA STRUCTURE 4: HASHMAP (Room Data) ---
// Stores all room information. Key = room ID, Value = Room struct.
unordered_map<int, Room> roomMap;

// --- DATA STRUCTURE 5: PRIORITY QUEUE (Dynamic Difficulty) ---
// Holds medium/hard questions sorted by difficulty (hardest first).
priority_queue<Question> difficultyPQ;

// --- TIMER & SCORE ---
int totalTime = 300;           // Total game time in seconds (chosen by player)
int timePenalty = 0;           // Accumulated penalty from wrong answers (in seconds)
int totalScore = 0;            // Current total score
bool gameActive = false;       // Is the game currently running?
time_t gameStartTime;          // When the game started (UNIX timestamp)

// --- QUESTION STORAGE ---
// After questions are assigned to rooms, store them here for quick lookup
unordered_map<int, Question> questionMap;  // Key = question ID

// --- HINT TRACKING ---
unordered_map<int, bool> hintUsed;  // Key = room ID, tracks if hint was used

// ============================================================
// PHASE 1: GRAPH FUNCTIONS
// ============================================================

// Build the room connection graph using an adjacency list.
// The 9 rooms form a 3x3 grid:
//   [1] --- [2] --- [3]
//    |       |       |
//   [4] --- [5] --- [6]
//    |       |       |
//   [7] --- [8] --- [9]
void initializeGraph() {
    // Clear any previous data
    for (int i = 0; i < 10; i++) {
        adjacencyList[i].clear();
    }

    // Room 1 (The Lobby) connects to Room 2 and Room 4
    adjacencyList[1] = {2, 4};

    // Room 2 (The Library) connects to Room 1, Room 3, and Room 5
    adjacencyList[2] = {1, 3, 5};

    // Room 3 (The Lab) connects to Room 2 and Room 6
    adjacencyList[3] = {2, 6};

    // Room 4 (The Workshop) connects to Room 1, Room 5, and Room 7
    adjacencyList[4] = {1, 5, 7};

    // Room 5 (The Hall) connects to Room 2, Room 4, Room 6, and Room 8
    adjacencyList[5] = {2, 4, 6, 8};

    // Room 6 (The Vault) connects to Room 3, Room 5, and Room 9
    adjacencyList[6] = {3, 5, 9};

    // Room 7 (The Dungeon) connects to Room 4 and Room 8
    adjacencyList[7] = {4, 8};

    // Room 8 (The Tower) connects to Room 5, Room 7, and Room 9
    adjacencyList[8] = {5, 7, 9};

    // Room 9 (The Escape) connects to Room 6 and Room 8
    adjacencyList[9] = {6, 8};
}

// Get all neighboring room IDs for a given room.
// Uses the adjacency list for O(degree) lookup.
vector<int> getNeighbors(int roomId) {
    if (roomId < 1 || roomId > 9) {
        return {};  // Invalid room ID
    }
    return adjacencyList[roomId];
}

// Unlock all rooms connected to the given room.
// Called after a player successfully completes a room.
void unlockNeighbors(int roomId) {
    vector<int> neighbors = getNeighbors(roomId);
    for (int i = 0; i < neighbors.size(); i++) {
        int neighborId = neighbors[i];
        // Only unlock if not already completed
        if (!roomMap[neighborId].isCompleted) {
            roomMap[neighborId].isLocked = false;
        }
    }
}

// ============================================================
// PHASE 1: HASHMAP FUNCTIONS
// ============================================================

// Initialize all 9 rooms with their default data.
// Room 1 starts unlocked, all others are locked.
void initializeRooms() {
    // Clear previous room data
    roomMap.clear();

    // Easy rooms (difficulty 1)
    roomMap[1] = {1, "The Lobby",    1, false, false, -1, 0};  // UNLOCKED (start room)
    roomMap[2] = {2, "The Library",  1, true,  false, -1, 0};  // locked
    roomMap[4] = {4, "The Workshop", 1, true,  false, -1, 0};  // locked

    // Medium rooms (difficulty 2)
    roomMap[3] = {3, "The Lab",      2, true,  false, -1, 0};  // locked
    roomMap[5] = {5, "The Hall",     2, true,  false, -1, 0};  // locked
    roomMap[7] = {7, "The Dungeon",  2, true,  false, -1, 0};  // locked

    // Hard rooms (difficulty 3)
    roomMap[6] = {6, "The Vault",    3, true,  false, -1, 0};  // locked
    roomMap[8] = {8, "The Tower",    3, true,  false, -1, 0};  // locked
    roomMap[9] = {9, "The Escape",   3, true,  false, -1, 0};  // locked
}

// ============================================================
// JSON HELPER FUNCTIONS
// (Manual JSON building — no external library needed)
// ============================================================

// Escape special characters in a string for safe JSON output
string jsonEscape(const string& input) {
    string result = "";
    for (int i = 0; i < input.size(); i++) {
        char c = input[i];
        if (c == '"') {
            result += "\\\"";
        } else if (c == '\\') {
            result += "\\\\";
        } else if (c == '\n') {
            result += "\\n";
        } else {
            result += c;
        }
    }
    return result;
}

// Convert a single Room to a JSON string
string roomToJson(const Room& room) {
    string json = "{";
    json += "\"id\":" + to_string(room.id) + ",";
    json += "\"name\":\"" + jsonEscape(room.name) + "\",";
    json += "\"difficulty\":" + to_string(room.difficulty) + ",";
    json += "\"isLocked\":" + string(room.isLocked ? "true" : "false") + ",";
    json += "\"isCompleted\":" + string(room.isCompleted ? "true" : "false") + ",";
    json += "\"score\":" + to_string(room.score) + ",";

    // Add the list of connected rooms (from adjacency list)
    json += "\"neighbors\":[";
    vector<int> neighbors = adjacencyList[room.id];
    for (int i = 0; i < neighbors.size(); i++) {
        json += to_string(neighbors[i]);
        if (i < neighbors.size() - 1) json += ",";
    }
    json += "]";

    json += "}";
    return json;
}

// Convert all rooms to a JSON array
string allRoomsToJson() {
    string json = "[";
    for (int i = 1; i <= 9; i++) {
        json += roomToJson(roomMap[i]);
        if (i < 9) json += ",";
    }
    json += "]";
    return json;
}

// ============================================================
// QUESTION MANAGEMENT (Preparing for Phase 2)
// These functions use the Queue and Priority Queue
// ============================================================

// Load all questions, shuffle them, and distribute to queue and priority queue.
void loadAndShuffleQuestions() {
    // Clear previous state
    while (!questionQueue.empty()) questionQueue.pop();
    while (!difficultyPQ.empty()) difficultyPQ.pop();
    questionMap.clear();
    hintUsed.clear();

    // Copy all questions from the question bank
    vector<Question> allQuestions;
    for (int i = 0; i < ALL_QUESTIONS.size(); i++) {
        Question q;
        q.id = i + 1;  // Question IDs start at 1
        q.text = ALL_QUESTIONS[i].text;
        q.hint = ALL_QUESTIONS[i].hint;
        q.options = ALL_QUESTIONS[i].options;
        q.correctOption = ALL_QUESTIONS[i].correctOption;
        q.difficulty = ALL_QUESTIONS[i].difficulty;
        q.points = ALL_QUESTIONS[i].points;
        allQuestions.push_back(q);
    }

    // Shuffle the questions randomly (Fisher-Yates shuffle)
    srand((unsigned int)time(NULL));
    for (int i = allQuestions.size() - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        swap(allQuestions[i], allQuestions[j]);
    }

    // Separate into easy (queue) and medium/hard (priority queue)
    for (int i = 0; i < allQuestions.size(); i++) {
        // Store in questionMap for quick lookup later
        questionMap[allQuestions[i].id] = allQuestions[i];

        if (allQuestions[i].difficulty == 1) {
            // Easy questions go into the regular QUEUE (FIFO)
            questionQueue.push(allQuestions[i]);
        } else {
            // Medium and hard questions go into the PRIORITY QUEUE
            // The PQ automatically sorts by difficulty (hard first)
            difficultyPQ.push(allQuestions[i]);
        }
    }
}

// Assign one question to each room.
// Easy rooms get questions from the queue, medium/hard from the priority queue.
void assignQuestionsToRooms() {
    for (int i = 1; i <= 9; i++) {
        Room& room = roomMap[i];

        if (room.difficulty == 1) {
            // Easy room: take from the regular QUEUE (FIFO order)
            if (!questionQueue.empty()) {
                Question q = questionQueue.front();
                questionQueue.pop();
                room.questionId = q.id;
            }
        } else {
            // Medium/Hard room: take from the PRIORITY QUEUE (hardest first)
            if (!difficultyPQ.empty()) {
                Question q = difficultyPQ.top();
                difficultyPQ.pop();
                room.questionId = q.id;
            }
        }

        // Initialize hint tracking for this room
        hintUsed[i] = false;
    }
}

// ============================================================
// GAME CONTROL FUNCTIONS
// ============================================================

// Calculate how many seconds are remaining
int getTimeRemaining() {
    if (!gameActive) return 0;

    // Calculate elapsed time since game started
    int elapsed = (int)(time(NULL) - gameStartTime);

    // Remaining = total time - elapsed - accumulated penalties
    int remaining = totalTime - elapsed - timePenalty;

    // If time is up, end the game
    if (remaining <= 0) {
        gameActive = false;
        return 0;
    }

    return remaining;
}

// Start a new game with the chosen timer duration
void startGame(int chosenTime) {
    // Set the timer to the player's choice (60-300 seconds)
    totalTime = chosenTime;
    timePenalty = 0;
    totalScore = 0;
    gameActive = true;

    // Clear the completion stack
    while (!completedStack.empty()) {
        completedStack.pop();
    }

    // Initialize the graph (room connections)
    initializeGraph();

    // Initialize the rooms (room data in hashmap)
    initializeRooms();

    // Load, shuffle, and assign questions
    loadAndShuffleQuestions();
    assignQuestionsToRooms();

    // Record the start time for the timer
    gameStartTime = time(NULL);
}

// Build the full game state as a JSON string
string getGameStateJson() {
    int timeLeft = getTimeRemaining();

    string json = "{";
    json += "\"gameActive\":" + string(gameActive ? "true" : "false") + ",";
    json += "\"timeRemaining\":" + to_string(timeLeft) + ",";
    json += "\"totalScore\":" + to_string(totalScore) + ",";
    json += "\"totalTime\":" + to_string(totalTime) + ",";

    // Count completed rooms
    int completedCount = 0;
    for (int i = 1; i <= 9; i++) {
        if (roomMap[i].isCompleted) completedCount++;
    }
    json += "\"completedRooms\":" + to_string(completedCount) + ",";

    // Include all room data
    json += "\"rooms\":" + allRoomsToJson();
    json += "}";

    return json;
}

// Get details for a specific room (including its question)
string getRoomDetailsJson(int roomId) {
    // Validate room ID
    if (roomId < 1 || roomId > 9) {
        return "{\"error\":\"Invalid room ID\"}";
    }

    Room& room = roomMap[roomId];

    // Check if room is locked
    if (room.isLocked) {
        return "{\"error\":\"Room is locked\",\"roomId\":" + to_string(roomId) + "}";
    }

    string json = "{";
    json += "\"roomId\":" + to_string(room.id) + ",";
    json += "\"name\":\"" + jsonEscape(room.name) + "\",";
    json += "\"difficulty\":" + to_string(room.difficulty) + ",";
    json += "\"isCompleted\":" + string(room.isCompleted ? "true" : "false") + ",";
    json += "\"score\":" + to_string(room.score) + ",";

    // Include the question if the room has one and is not completed
    if (room.questionId != -1 && questionMap.count(room.questionId) > 0) {
        Question& q = questionMap[room.questionId];
        json += "\"question\":\"" + jsonEscape(q.text) + "\",";
        json += "\"options\":[";
        for (int i = 0; i < q.options.size(); i++) {
            json += "\"" + jsonEscape(q.options[i]) + "\"";
            if (i < q.options.size() - 1) json += ",";
        }
        json += "],";
        json += "\"points\":" + to_string(q.points) + ",";
        json += "\"hintUsed\":" + string(hintUsed[roomId] ? "true" : "false");
    }

    json += "}";
    return json;
}

// Process an answer submission
string submitAnswer(int roomId, int answer) {
    // Validate inputs
    if (!gameActive) {
        return "{\"error\":\"Game is not active\"}";
    }
    if (roomId < 1 || roomId > 9) {
        return "{\"error\":\"Invalid room ID\"}";
    }

    Room& room = roomMap[roomId];

    // Check if room is accessible
    if (room.isLocked) {
        return "{\"error\":\"Room is locked\"}";
    }
    if (room.isCompleted) {
        return "{\"error\":\"Room already completed\"}";
    }

    // Get the question assigned to this room
    Question& question = questionMap[room.questionId];

    if (answer == question.correctOption) {
        // ---- CORRECT ANSWER ----

        // Calculate points (subtract 5 if hint was used)
        int pointsEarned = question.points;
        if (hintUsed[roomId]) {
            pointsEarned = max(0, pointsEarned - 5);
        }

        // Mark room as completed
        room.isCompleted = true;
        room.score = pointsEarned;

        // Update total score
        totalScore += pointsEarned;

        // Check if this is the final room (Room 9) — bonus!
        if (roomId == 9) {
            totalScore += 50;  // Completion bonus
        }

        // STACK: Push the completed room ID onto the stack
        completedStack.push(roomId);

        // GRAPH: Unlock neighboring rooms
        unlockNeighbors(roomId);

        // Build the response
        string json = "{";
        json += "\"correct\":true,";
        json += "\"pointsEarned\":" + to_string(pointsEarned) + ",";
        if (roomId == 9) {
            json += "\"bonus\":50,";
        }
        json += "\"totalScore\":" + to_string(totalScore) + ",";
        json += "\"timeRemaining\":" + to_string(getTimeRemaining()) + ",";

        // List the rooms that were unlocked
        json += "\"unlockedRooms\":[";
        vector<int> neighbors = getNeighbors(roomId);
        bool first = true;
        for (int i = 0; i < neighbors.size(); i++) {
            if (!roomMap[neighbors[i]].isCompleted) {
                if (!first) json += ",";
                json += to_string(neighbors[i]);
                first = false;
            }
        }
        json += "],";

        json += "\"message\":\"Correct! +" + to_string(pointsEarned) + " points!\"";
        json += "}";
        return json;

    } else {
        // ---- WRONG ANSWER ----

        // Apply 5-second time penalty
        timePenalty += 5;

        int timeLeft = getTimeRemaining();

        string json = "{";
        json += "\"correct\":false,";
        json += "\"timePenalty\":5,";
        json += "\"timeRemaining\":" + to_string(timeLeft) + ",";
        json += "\"message\":\"Wrong! -5 seconds. Try again!\"";
        json += "}";
        return json;
    }
}

// Get hint for a room
string getHintJson(int roomId) {
    if (roomId < 1 || roomId > 9) {
        return "{\"error\":\"Invalid room ID\"}";
    }

    Room& room = roomMap[roomId];
    if (room.questionId == -1) {
        return "{\"error\":\"No question assigned\"}";
    }

    Question& q = questionMap[room.questionId];

    // Mark hint as used (costs 5 points from reward)
    hintUsed[roomId] = true;

    string json = "{";
    json += "\"hint\":\"" + jsonEscape(q.hint) + "\",";
    json += "\"roomId\":" + to_string(roomId) + ",";
    json += "\"pointPenalty\":5";
    json += "}";
    return json;
}

// Get completion history from the stack (LIFO order)
string getHistoryJson() {
    // Make a copy of the stack so we don't destroy it
    stack<int> tempStack = completedStack;

    string json = "{";

    // Build the history array by popping the stack copy
    json += "\"history\":[";
    bool first = true;
    while (!tempStack.empty()) {
        int roomId = tempStack.top();
        tempStack.pop();

        if (!first) json += ",";
        json += "{";
        json += "\"roomId\":" + to_string(roomId) + ",";
        json += "\"name\":\"" + jsonEscape(roomMap[roomId].name) + "\",";
        json += "\"score\":" + to_string(roomMap[roomId].score);
        json += "}";
        first = false;
    }
    json += "],";

    json += "\"totalScore\":" + to_string(totalScore) + ",";
    json += "\"totalTime\":" + to_string(totalTime) + ",";

    // Calculate time spent
    int timeSpent = 0;
    if (gameStartTime > 0) {
        timeSpent = (int)(time(NULL) - gameStartTime);
        if (timeSpent > totalTime) timeSpent = totalTime;
    }
    json += "\"timeSpent\":" + to_string(timeSpent);

    json += "}";
    return json;
}

// ============================================================
// SIMPLE JSON PARSER (for POST request body)
// Parses {"roomId": 1, "answer": 2} — nothing more complex
// ============================================================

// Extract an integer value for a given key from a simple JSON string
int getJsonInt(const string& json, const string& key) {
    // Find the key in the JSON string
    string searchKey = "\"" + key + "\"";
    size_t keyPos = json.find(searchKey);
    if (keyPos == string::npos) return -1;

    // Find the colon after the key
    size_t colonPos = json.find(':', keyPos);
    if (colonPos == string::npos) return -1;

    // Extract the number after the colon
    string numberStr = "";
    for (size_t i = colonPos + 1; i < json.size(); i++) {
        char c = json[i];
        if (c >= '0' && c <= '9') {
            numberStr += c;
        } else if (numberStr.size() > 0) {
            break;  // End of number
        }
    }

    if (numberStr.empty()) return -1;
    return stoi(numberStr);
}

// ============================================================
// HTTP SERVER (Phase 0) — Main entry point
// ============================================================

int main() {
    // Create the HTTP server
    httplib::Server server;

    // ---- SERVE FRONTEND FILES ----
    // This serves index.html, style.css, game.js from the frontend folder
    server.set_mount_point("/", "../frontend");

    // ---- CORS HEADERS ----
    // Allow the frontend to communicate with the backend
    server.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });

    // Handle preflight OPTIONS requests
    server.Options(".*", [](const httplib::Request& req, httplib::Response& res) {
        res.set_content("", "text/plain");
    });

    // ---- API ENDPOINT: Start Game ----
    // GET /api/start?time=300
    // Starts a new game with the chosen timer duration
    server.Get("/api/start", [](const httplib::Request& req, httplib::Response& res) {
        // Get the timer duration from the query parameter (default 300 seconds)
        int chosenTime = 300;
        if (req.has_param("time")) {
            chosenTime = stoi(req.get_param_value("time"));
            // Clamp to valid range (60-300 seconds)
            if (chosenTime < 60) chosenTime = 60;
            if (chosenTime > 300) chosenTime = 300;
        }

        // Start the game
        startGame(chosenTime);

        // Return the initial game state
        string response = getGameStateJson();
        res.set_content(response, "application/json");
    });

    // ---- API ENDPOINT: Get Game State ----
    // GET /api/state
    // Returns current game state (called every second by the frontend)
    server.Get("/api/state", [](const httplib::Request& req, httplib::Response& res) {
        string response = getGameStateJson();
        res.set_content(response, "application/json");
    });

    // ---- API ENDPOINT: Get Room Details ----
    // GET /api/room/:id
    // Returns room details including the puzzle question
    server.Get(R"(/api/room/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        int roomId = stoi(req.matches[1]);
        string response = getRoomDetailsJson(roomId);
        res.set_content(response, "application/json");
    });

    // ---- API ENDPOINT: Submit Answer ----
    // POST /api/answer
    // Body: {"roomId": 1, "answer": 2}
    server.Post("/api/answer", [](const httplib::Request& req, httplib::Response& res) {
        // Parse the request body
        int roomId = getJsonInt(req.body, "roomId");
        int answer = getJsonInt(req.body, "answer");

        if (roomId == -1 || answer == -1) {
            res.set_content("{\"error\":\"Invalid request. Need roomId and answer.\"}", "application/json");
            return;
        }

        // Process the answer
        string response = submitAnswer(roomId, answer);
        res.set_content(response, "application/json");
    });

    // ---- API ENDPOINT: Get Hint ----
    // GET /api/hint/:roomId
    // Returns the hint for a room (costs 5 points)
    server.Get(R"(/api/hint/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        int roomId = stoi(req.matches[1]);
        string response = getHintJson(roomId);
        res.set_content(response, "application/json");
    });

    // ---- API ENDPOINT: Get History ----
    // GET /api/history
    // Returns completion history from the stack (LIFO order)
    server.Get("/api/history", [](const httplib::Request& req, httplib::Response& res) {
        string response = getHistoryJson();
        res.set_content(response, "application/json");
    });

    // ---- START THE SERVER ----
    cout << "============================================" << endl;
    cout << "  EscapeX Game Server" << endl;
    cout << "  Open http://localhost:8080 in your browser" << endl;
    cout << "  Press Ctrl+C to stop" << endl;
    cout << "============================================" << endl;

    server.listen("localhost", 8080);

    return 0;
}
