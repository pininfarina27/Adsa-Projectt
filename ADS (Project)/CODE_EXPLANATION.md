# EscapeX - Detailed Code Explanation

> A line-by-line logical walkthrough of the entire game.
> Read this to understand every piece of the project.

---

## Table of Contents

1. [How the Whole System Works](#1-how-the-whole-system-works)
2. [game_server.cpp — Complete Walkthrough](#2-game_servercpp--complete-walkthrough)
   - [Includes and Setup](#21-includes-and-setup)
   - [Structs (Data Holders)](#22-structs-data-holders)
   - [Global Game State (The 5 Data Structures)](#23-global-game-state-the-5-data-structures)
   - [Graph Functions](#24-graph-functions)
   - [HashMap Functions](#25-hashmap-functions)
   - [JSON Helper Functions](#26-json-helper-functions)
   - [Question Management (Queue + Priority Queue)](#27-question-management-queue--priority-queue)
   - [Game Control Functions](#28-game-control-functions)
   - [Answer Processing](#29-answer-processing)
   - [Hint System](#210-hint-system)
   - [Completion History (Stack)](#211-completion-history-stack)
   - [JSON Parser](#212-json-parser)
   - [HTTP Server (main function)](#213-http-server-main-function)
3. [questions.h — Question Bank](#3-questionsh--question-bank)
4. [game.js — Frontend Logic](#4-gamejs--frontend-logic)
5. [Data Structure Summary](#5-data-structure-summary)
6. [Game Flow: What Happens When You Play](#6-game-flow-what-happens-when-you-play)

---

## 1. How the Whole System Works

```
BROWSER (what you see)              C++ SERVER (what does the thinking)
========================            =================================
                                    
index.html  ←→  game.js   ←HTTP→   game_server.cpp
  (layout)      (actions)           (all game logic + data structures)
                                         ↓
style.css                           questions.h
  (looks)                           (30 hardcoded questions)
```

**The flow is simple:**
1. You open `http://localhost:8080` in your browser
2. The C++ server sends you `index.html`, `style.css`, and `game.js`
3. When you click buttons, `game.js` sends requests to the C++ server
4. The C++ server processes the request using the 5 data structures
5. The C++ server sends back a JSON response
6. `game.js` updates the page based on the response

**All data structure operations happen in C++.** The browser just displays things.

---

## 2. game_server.cpp — Complete Walkthrough

### 2.1 Includes and Setup

```cpp
#include "httplib.h"      // The HTTP server library (single file, handles web requests)
#include "questions.h"    // Our question bank (30 questions)

#include <vector>         // For adjacency list (Graph)
#include <stack>          // For completion history (Stack)
#include <queue>          // For question pool (Queue) and difficulty (Priority Queue)
#include <unordered_map>  // For room data (HashMap)
#include <string>         // For text handling
#include <ctime>          // For timer (time(NULL) gives current time in seconds)
#include <cstdlib>        // For random number generation (srand, rand)

using namespace std;      // So we don't have to write std:: everywhere
```

**Why these includes?** Each one corresponds to a data structure or utility we need. There are no unnecessary imports.

---

### 2.2 Structs (Data Holders)

#### Question Struct
```cpp
struct Question {
    int id;                    // Unique ID (1-30)
    string text;               // "What is the time complexity of...?"
    string hint;               // "Think about direct memory access"
    vector<string> options;    // {"O(n)", "O(1)", "O(log n)", "O(n^2)"}
    int correctOption;         // 1 (meaning options[1] = "O(1)" is correct)
    int difficulty;            // 1=easy, 2=medium, 3=hard
    int points;                // 10, 20, or 30

    // This operator is REQUIRED for the priority queue to work.
    // It tells C++: "Question A has LOWER priority than Question B
    // if A's difficulty is LESS than B's difficulty."
    // Since priority_queue is a MAX-HEAP, the HIGHEST difficulty
    // comes out first. This is how we get hard questions first.
    bool operator<(const Question& other) const {
        return difficulty < other.difficulty;
    }
};
```

**Why a struct and not a class?** A struct is simpler — all fields are public by default. We don't need encapsulation, getters, setters, or any OOP complexity. It's just a container for data.

**Why `operator<`?** The C++ `priority_queue` uses this comparison to decide which element has higher priority. By saying "lower difficulty = lower priority," we ensure that **hard questions (difficulty=3) come out before easy ones (difficulty=1)**.

#### Room Struct
```cpp
struct Room {
    int id;            // 1 through 9
    string name;       // "The Lobby", "The Library", etc.
    int difficulty;    // 1=easy, 2=medium, 3=hard
    bool isLocked;     // true = player cannot enter
    bool isCompleted;  // true = player already solved this room
    int questionId;    // Which question is assigned (-1 = none yet)
    int score;         // Points the player earned here (0 if not completed)
};
```

**What each field does in gameplay:**
- `isLocked` starts as `true` for all rooms except Room 1. When a neighbor is solved, it becomes `false`.
- `isCompleted` starts as `false`. When the player answers correctly, it becomes `true`.
- `questionId` is set to `-1` at first. During game start, a question is assigned and this gets the question's ID.
- `score` is `0` until the player solves the room. Then it's set to the points earned.

---

### 2.3 Global Game State (The 5 Data Structures)

This is the heart of the project. Each variable is a data structure:

```cpp
// DATA STRUCTURE 1: GRAPH (Adjacency List)
vector<vector<int>> adjacencyList(10);
```
**What is it?** A vector of vectors. `adjacencyList[1]` = `{2, 4}` means Room 1 connects to Room 2 and Room 4.

**Why 10?** Rooms are numbered 1-9. Index 0 is unused. We allocate 10 slots so we can access `adjacencyList[9]` directly without offset math.

**Why adjacency list over adjacency matrix?** With 9 rooms, a matrix would be 9×9 = 81 cells (mostly zeros). An adjacency list only stores actual connections — 24 entries total for our grid. It's simpler and more memory-efficient.

```cpp
// DATA STRUCTURE 2: STACK
stack<int> completedStack;
```
**What is it?** A LIFO (Last In, First Out) container. We push a room ID every time the player solves a room. At game end, we pop them out to show history in reverse order — "most recently solved first."

**Why a stack?** Because we want reverse-chronological order. If the player solved rooms 1 → 4 → 5 → 8, popping gives us 8 → 5 → 4 → 1. That's exactly the "completion history" we show on the results screen.

```cpp
// DATA STRUCTURE 3: QUEUE
queue<Question> questionQueue;
```
**What is it?** A FIFO (First In, First Out) container. At game start, easy questions are shuffled and pushed in. When an easy room needs a question, we take from the front.

**Why a queue?** Because we want to use each question exactly once, in shuffled order. Dequeuing from the front guarantees no repeats — once a question is taken, it's gone.

```cpp
// DATA STRUCTURE 4: HASHMAP
unordered_map<int, Room> roomMap;
```
**What is it?** A key-value store. Key = room ID (integer), Value = Room struct. Looking up `roomMap[5]` gives us all data about Room 5 in O(1) average time.

**Why a hashmap?** Because we need to look up, read, and update room data constantly. Every time the player clicks a room, answers a question, or we need to check neighbors — we access `roomMap`. O(1) access makes this instant.

```cpp
// DATA STRUCTURE 5: PRIORITY QUEUE
priority_queue<Question> difficultyPQ;
```
**What is it?** A max-heap that automatically sorts questions by difficulty. When we call `difficultyPQ.top()`, we get the hardest available question. When we call `difficultyPQ.pop()`, that question is removed.

**Why a priority queue?** Because medium/hard rooms need harder questions. Instead of manually sorting or searching, the priority queue automatically gives us the hardest question available. This creates a natural difficulty curve — harder rooms get harder questions.

```cpp
// Timer & Score
int totalTime = 300;       // How long the game lasts (player's choice)
int timePenalty = 0;        // Total seconds lost to wrong answers
int totalScore = 0;         // Player's current score
bool gameActive = false;    // Is a game currently running?
time_t gameStartTime;       // UNIX timestamp when the game started
```

**How the timer works:** We record `gameStartTime` when the game starts. To know remaining time, we calculate:
```
remaining = totalTime - (currentTime - gameStartTime) - timePenalty
```
This is checked every time the frontend polls `/api/state`. When `remaining <= 0`, the game ends.

---

### 2.4 Graph Functions

#### `initializeGraph()`

```cpp
void initializeGraph() {
    // Clear any data from a previous game
    for (int i = 0; i < 10; i++) {
        adjacencyList[i].clear();
    }

    // Build the 3x3 grid connections:
    //   [1] --- [2] --- [3]
    //    |       |       |
    //   [4] --- [5] --- [6]
    //    |       |       |
    //   [7] --- [8] --- [9]

    adjacencyList[1] = {2, 4};        // Room 1 → Room 2, Room 4
    adjacencyList[2] = {1, 3, 5};     // Room 2 → Room 1, Room 3, Room 5
    adjacencyList[3] = {2, 6};        // Room 3 → Room 2, Room 6
    adjacencyList[4] = {1, 5, 7};     // Room 4 → Room 1, Room 5, Room 7
    adjacencyList[5] = {2, 4, 6, 8};  // Room 5 → 4 connections (central hub)
    adjacencyList[6] = {3, 5, 9};     // Room 6 → Room 3, Room 5, Room 9
    adjacencyList[7] = {4, 8};        // Room 7 → Room 4, Room 8
    adjacencyList[8] = {5, 7, 9};     // Room 8 → Room 5, Room 7, Room 9
    adjacencyList[9] = {6, 8};        // Room 9 → Room 6, Room 8
}
```

**Key insight:** This graph is **undirected** — if Room 1 connects to Room 2, then Room 2 also connects to Room 1. You can see this in the lists: `adjacencyList[1]` contains `2`, and `adjacencyList[2]` contains `1`.

#### `getNeighbors(roomId)`

```cpp
vector<int> getNeighbors(int roomId) {
    if (roomId < 1 || roomId > 9) return {};  // Safety check
    return adjacencyList[roomId];              // Direct lookup — O(1)
}
```

Simply returns the list of connected rooms. For Room 5, this returns `{2, 4, 6, 8}`.

#### `unlockNeighbors(roomId)`

```cpp
void unlockNeighbors(int roomId) {
    vector<int> neighbors = getNeighbors(roomId);  // Get connected rooms
    for (int i = 0; i < neighbors.size(); i++) {
        int neighborId = neighbors[i];
        if (!roomMap[neighborId].isCompleted) {     // Don't re-lock completed rooms
            roomMap[neighborId].isLocked = false;    // Unlock the neighbor
        }
    }
}
```

**When is this called?** Every time the player correctly answers a room's puzzle. If they solve Room 1, this function unlocks Room 2 and Room 4 (Room 1's neighbors in the graph).

**Why the `isCompleted` check?** If Room 2 is already completed, we don't want to change its state. It should stay "completed," not go back to "unlocked."

---

### 2.5 HashMap Functions

#### `initializeRooms()`

```cpp
void initializeRooms() {
    roomMap.clear();  // Remove previous game data

    // Easy rooms (difficulty 1, worth 10 points)
    roomMap[1] = {1, "The Lobby",    1, false, false, -1, 0};  // UNLOCKED
    roomMap[2] = {2, "The Library",  1, true,  false, -1, 0};  // locked
    roomMap[4] = {4, "The Workshop", 1, true,  false, -1, 0};  // locked

    // Medium rooms (difficulty 2, worth 20 points)
    roomMap[3] = {3, "The Lab",      2, true,  false, -1, 0};
    roomMap[5] = {5, "The Hall",     2, true,  false, -1, 0};
    roomMap[7] = {7, "The Dungeon",  2, true,  false, -1, 0};

    // Hard rooms (difficulty 3, worth 30 points)
    roomMap[6] = {6, "The Vault",    3, true,  false, -1, 0};
    roomMap[8] = {8, "The Tower",    3, true,  false, -1, 0};
    roomMap[9] = {9, "The Escape",   3, true,  false, -1, 0};
}
```

**Reading the initialization:** `{1, "The Lobby", 1, false, false, -1, 0}` means:
- `id=1`, `name="The Lobby"`, `difficulty=1` (easy)
- `isLocked=false` (Room 1 is the START room — always unlocked)
- `isCompleted=false`, `questionId=-1` (no question yet), `score=0`

**Notice:** Only Room 1 has `isLocked=false`. All other rooms start locked. The player must solve rooms to unlock paths through the graph.

---

### 2.6 JSON Helper Functions

Since we don't use a JSON library (to keep things simple), we build JSON strings manually.

#### `jsonEscape(input)`
Handles special characters that would break JSON. For example, a quote `"` inside a string must become `\"`.

#### `roomToJson(room)`
Converts a Room struct to a JSON string like:
```json
{"id":1,"name":"The Lobby","difficulty":1,"isLocked":false,"isCompleted":false,"score":0,"neighbors":[2,4]}
```

Notice it also includes the **neighbors** from the adjacency list. This is how the frontend knows which rooms are connected — it uses Graph data inside the HashMap response.

#### `allRoomsToJson()`
Loops through rooms 1-9 and builds a JSON array of all rooms.

---

### 2.7 Question Management (Queue + Priority Queue)

#### `loadAndShuffleQuestions()`

This function does 4 things:

**Step 1: Clear previous game data**
```cpp
while (!questionQueue.empty()) questionQueue.pop();   // Empty the queue
while (!difficultyPQ.empty()) difficultyPQ.pop();     // Empty the priority queue
questionMap.clear();                                   // Clear question lookup
```

**Step 2: Copy questions from the bank**
```cpp
vector<Question> allQuestions;
for (int i = 0; i < ALL_QUESTIONS.size(); i++) {
    Question q;
    q.id = i + 1;  // Assign IDs 1-30
    // ... copy all fields from ALL_QUESTIONS[i]
    allQuestions.push_back(q);
}
```

**Step 3: Shuffle using Fisher-Yates algorithm**
```cpp
srand((unsigned int)time(NULL));  // Seed with current time for randomness
for (int i = allQuestions.size() - 1; i > 0; i--) {
    int j = rand() % (i + 1);         // Pick a random index from 0 to i
    swap(allQuestions[i], allQuestions[j]); // Swap current with random
}
```

**How Fisher-Yates works:** Start from the end. Pick a random element from the remaining unshuffled part. Swap it with the current position. Move left. Repeat. This gives every possible ordering an equal probability — a truly fair shuffle.

**Step 4: Distribute to Queue and Priority Queue**
```cpp
for (int i = 0; i < allQuestions.size(); i++) {
    questionMap[allQuestions[i].id] = allQuestions[i];  // Store for O(1) lookup

    if (allQuestions[i].difficulty == 1) {
        questionQueue.push(allQuestions[i]);    // Easy → regular QUEUE
    } else {
        difficultyPQ.push(allQuestions[i]);     // Medium/Hard → PRIORITY QUEUE
    }
}
```

**Why split them?** Easy rooms should get easy questions (from the queue — random order). Medium/hard rooms should get the hardest available questions (from the priority queue — automatic sorting).

#### `assignQuestionsToRooms()`

```cpp
for (int i = 1; i <= 9; i++) {
    Room& room = roomMap[i];

    if (room.difficulty == 1) {
        // Easy room → take from QUEUE (FIFO)
        Question q = questionQueue.front();  // Look at the front
        questionQueue.pop();                  // Remove from the front
        room.questionId = q.id;               // Assign to the room
    } else {
        // Medium/Hard room → take from PRIORITY QUEUE
        Question q = difficultyPQ.top();     // Get the highest-priority (hardest)
        difficultyPQ.pop();                   // Remove it
        room.questionId = q.id;
    }
}
```

**The result:** Each of the 9 rooms gets one unique question. Easy rooms get random easy questions. Hard rooms 6, 8, 9 get the 3 hardest questions available. Medium rooms 3, 5, 7 get the remaining medium questions.

---

### 2.8 Game Control Functions

#### `getTimeRemaining()`

```cpp
int getTimeRemaining() {
    if (!gameActive) return 0;

    int elapsed = (int)(time(NULL) - gameStartTime);  // Seconds since game started
    int remaining = totalTime - elapsed - timePenalty; // Subtract elapsed + penalties

    if (remaining <= 0) {
        gameActive = false;  // Game over!
        return 0;
    }
    return remaining;
}
```

**Example:** If `totalTime=300`, game started 100 seconds ago, and the player got 3 wrong answers (`timePenalty=15`):
```
remaining = 300 - 100 - 15 = 185 seconds left
```

#### `startGame(chosenTime)`

This function resets EVERYTHING for a fresh game:

```cpp
void startGame(int chosenTime) {
    totalTime = chosenTime;    // Set the timer (60-300 seconds)
    timePenalty = 0;           // Reset penalties
    totalScore = 0;            // Reset score
    gameActive = true;         // Game is now running

    // Clear the STACK (empty all previous completion records)
    while (!completedStack.empty()) completedStack.pop();

    initializeGraph();         // Rebuild the GRAPH
    initializeRooms();         // Rebuild the HASHMAP
    loadAndShuffleQuestions();  // Rebuild the QUEUE and PRIORITY QUEUE
    assignQuestionsToRooms();  // Connect questions to rooms

    gameStartTime = time(NULL); // Record the start time
}
```

**Notice:** Every data structure is rebuilt from scratch. This ensures "Play Again" works perfectly — no leftover state from the previous game.

---

### 2.9 Answer Processing

#### `submitAnswer(roomId, answer)` — The Core Game Logic

This is the most important function. It handles everything that happens when the player clicks an answer.

```cpp
string submitAnswer(int roomId, int answer) {
    // --- SAFETY CHECKS ---
    if (!gameActive) return error;    // Game is over
    if (roomId < 1 || roomId > 9) return error;  // Invalid room
    if (room.isLocked) return error;   // Room not accessible
    if (room.isCompleted) return error; // Already solved
```

**If the answer is CORRECT:**
```cpp
    // 1. Calculate points (minus 5 if hint was used)
    int pointsEarned = question.points;       // Base: 10, 20, or 30
    if (hintUsed[roomId]) {
        pointsEarned = max(0, pointsEarned - 5);  // Hint penalty
    }

    // 2. Mark room as completed (HASHMAP update)
    room.isCompleted = true;
    room.score = pointsEarned;

    // 3. Add to total score
    totalScore += pointsEarned;

    // 4. Bonus for completing Room 9
    if (roomId == 9) totalScore += 50;

    // 5. Push onto the STACK (completion history)
    completedStack.push(roomId);

    // 6. Unlock neighbors using the GRAPH
    unlockNeighbors(roomId);
```

**If the answer is WRONG:**
```cpp
    // Apply 5-second time penalty
    timePenalty += 5;
    // That's it. The player can try again immediately.
```

**All 5 data structures in action on a correct answer:**
1. **HashMap** — `roomMap[roomId]` is updated (isCompleted, score)
2. **Stack** — `completedStack.push(roomId)` records the completion
3. **Graph** — `unlockNeighbors(roomId)` checks adjacencyList to unlock connected rooms
4. **Queue** — already used to assign the question at game start
5. **Priority Queue** — already used to assign the question at game start

---

### 2.10 Hint System

```cpp
string getHintJson(int roomId) {
    Room& room = roomMap[roomId];             // HASHMAP lookup
    Question& q = questionMap[room.questionId]; // Get the question

    hintUsed[roomId] = true;   // Mark hint as used (costs 5 points later)

    // Return the hint text as JSON
    return "{\"hint\":\"" + q.hint + "\"}";
}
```

**The cost:** When `submitAnswer` checks `hintUsed[roomId]`, it subtracts 5 from the points earned. So a 10-point easy room with a hint gives only 5 points.

---

### 2.11 Completion History (Stack)

```cpp
string getHistoryJson() {
    stack<int> tempStack = completedStack;  // COPY the stack (don't destroy it)

    string json = "{\"history\":[";
    while (!tempStack.empty()) {
        int roomId = tempStack.top();     // Look at the top (most recent)
        tempStack.pop();                   // Remove it from the copy

        // Add this room's info to the JSON array
        json += "{\"roomId\":" + roomId + ",\"name\":\"" + roomMap[roomId].name + "\"}";
    }
    json += "]}";
    return json;
}
```

**Why copy the stack?** If we popped the real `completedStack`, we'd lose the data. By copying it into `tempStack`, we can pop and read without destroying the original.

**The LIFO result:** If rooms were completed in order 1 → 2 → 5 → 9, the stack pops them as 9 → 5 → 2 → 1. The history shows the most recently completed room first — exactly what we want.

---

### 2.12 JSON Parser

```cpp
int getJsonInt(const string& json, const string& key) {
    // For a JSON like: {"roomId": 1, "answer": 2}
    // getJsonInt(json, "roomId") returns 1
    // getJsonInt(json, "answer") returns 2

    // 1. Find the key (e.g., "roomId") in the string
    // 2. Find the colon after it
    // 3. Read the digits after the colon
    // 4. Convert to integer and return
}
```

**Why manual parsing?** To avoid adding another library. We only need to parse two integers from a simple JSON object. This 20-line function handles that perfectly.

---

### 2.13 HTTP Server (main function)

```cpp
int main() {
    httplib::Server server;

    // Serve frontend files (HTML, CSS, JS) from the frontend folder
    server.set_mount_point("/", "../frontend");

    // Set CORS headers so the browser allows API calls
    server.set_default_headers({
        {"Access-Control-Allow-Origin", "*"}
    });
```

**API Endpoints — each one maps to a game function:**

| Endpoint | Method | What it does |
|---|---|---|
| `/api/start?time=300` | GET | Calls `startGame(300)`, returns initial state |
| `/api/state` | GET | Calls `getGameStateJson()`, returns timer + all rooms |
| `/api/room/5` | GET | Calls `getRoomDetailsJson(5)`, returns room + question |
| `/api/answer` | POST | Calls `submitAnswer(roomId, answer)`, returns result |
| `/api/hint/3` | GET | Calls `getHintJson(3)`, returns hint text |
| `/api/history` | GET | Calls `getHistoryJson()`, pops stack for history |

```cpp
    // Start listening on localhost:8080
    server.listen("localhost", 8080);
```

The server runs in a loop, waiting for requests. When a request comes in, it calls the matching handler function, which uses the data structures to compute the response.

---

## 3. questions.h — Question Bank

This file contains 30 questions as a simple vector of structs:

```cpp
vector<QuestionData> ALL_QUESTIONS = {
    // Each entry: {text, hint, {option1, option2, option3, option4}, correctIndex, difficulty, points}
    {
        "What is the time complexity of accessing an array element by index?",
        "Arrays store elements in contiguous memory",
        {"O(n)", "O(1)", "O(log n)", "O(n^2)"},
        1,      // Answer: O(1) — which is options[1]
        1, 10   // Easy, 10 points
    },
    // ... 29 more questions
};
```

**Question distribution:**
- Questions 1-10: Easy (difficulty=1, 10 points) — basic DSA, logic riddles
- Questions 11-20: Medium (difficulty=2, 20 points) — math patterns, intermediate DSA
- Questions 21-30: Hard (difficulty=3, 30 points) — complex logic, advanced DSA

---

## 4. game.js — Frontend Logic

The JavaScript file handles everything the player sees and interacts with.

#### `startGame()`
1. Reads the timer dropdown value
2. Sends `GET /api/start?time=X` to the C++ server
3. Switches from start screen to game screen
4. Updates the room map colors
5. Starts polling the server every 1 second for timer updates

#### `startTimerPolling()`
Every 1000ms (1 second):
1. Sends `GET /api/state` to get the current time remaining
2. Updates the timer display (formats seconds into M:SS)
3. Changes timer color: normal → orange (≤30s) → red blinking (≤10s)
4. If `gameActive` is false, stops polling and shows the results screen

#### `updateRoomMap(rooms)`
For each of the 9 rooms, sets the CSS class based on state:
- `locked` → gray, can't click
- `unlocked` → blue, clickable
- `completed` → green
- `current` → orange border (the room you're viewing)

#### `enterRoom(roomId)`
1. Sends `GET /api/room/X` to get the room's question
2. If locked → shows error message
3. If completed → shows "already solved" message
4. Otherwise → displays the question with 4 answer buttons

#### `submitAnswer(optionIndex)`
1. Disables all buttons (prevents double-clicking)
2. Sends `POST /api/answer` with `{roomId, answer}`
3. If correct → flashes green, shows "+X points!", waits 1.2s, then refreshes map
4. If wrong → flashes red, shows "-5 seconds", waits 0.8s, re-enables buttons

#### `showHint()`
1. Sends `GET /api/hint/X`
2. Displays the hint text below the question
3. Disables the hint button (can only use once)
4. Updates the displayed points (-5)

#### `showResults()`
1. Sends `GET /api/history` to get the Stack data
2. Displays final score, rooms completed, time spent
3. Lists completion history in LIFO order (from the stack)

---

## 5. Data Structure Summary

### How Each Data Structure is Used

```
┌─────────────────────────────────────────────────────────────┐
│                    GAME START                                │
│                                                              │
│  1. GRAPH built (adjacencyList filled with room connections) │
│  2. HASHMAP filled (roomMap with 9 rooms, Room 1 unlocked)   │
│  3. Questions shuffled → QUEUE (easy) + PRIORITY QUEUE (hard)│
│  4. Questions assigned to rooms from Queue and PQ             │
│  5. STACK is empty (no rooms completed yet)                   │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│                  PLAYER SOLVES A ROOM                        │
│                                                              │
│  1. HASHMAP: roomMap[id].isCompleted = true                  │
│  2. STACK: completedStack.push(roomId)                       │
│  3. GRAPH: adjacencyList[id] → unlock neighbor rooms         │
│  4. HASHMAP: neighbor rooms' isLocked = false                │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│                    GAME END                                   │
│                                                              │
│  1. STACK: pop all → show history in reverse order           │
│  2. HASHMAP: read scores from each completed room            │
│  3. Total score = sum of all room scores + bonuses            │
└─────────────────────────────────────────────────────────────┘
```

### Time Complexity Table

| Operation | Data Structure | Time |
|---|---|---|
| Find room's neighbors | Graph (adjacency list) | O(1) to access, O(degree) to iterate |
| Look up room data | HashMap (unordered_map) | O(1) average |
| Push completed room | Stack | O(1) |
| Pop history entry | Stack | O(1) |
| Dequeue a question | Queue | O(1) |
| Get hardest question | Priority Queue | O(log n) |

---

## 6. Game Flow: What Happens When You Play

### Step-by-step with data structures

**1. Player clicks "Start Game" (5 minutes)**
```
Frontend: fetch("/api/start?time=300")
Backend:  startGame(300)
          → initializeGraph()     — GRAPH: 9 rooms, 24 edges
          → initializeRooms()     — HASHMAP: 9 rooms stored
          → loadAndShuffleQuestions() — QUEUE: 10 easy, PQ: 20 medium/hard
          → assignQuestionsToRooms() — QUEUE.pop × 3, PQ.pop × 6
          → gameStartTime = now
```

**2. Player clicks Room 1 (unlocked, blue)**
```
Frontend: fetch("/api/room/1")
Backend:  roomMap[1] → isLocked=false, questionId=7
          questionMap[7] → "What comes next: 1,1,2,3,5,8,?"
          Return: {question, options, points:10}
```

**3. Player clicks wrong answer**
```
Frontend: fetch("/api/answer", {roomId:1, answer:0})
Backend:  questionMap[7].correctOption = 2, player said 0
          → WRONG! timePenalty += 5
          Return: {correct:false, timePenalty:5}
```

**4. Player clicks correct answer**
```
Frontend: fetch("/api/answer", {roomId:1, answer:2})
Backend:  answer == correctOption → CORRECT!
          → roomMap[1].isCompleted = true         — HASHMAP
          → roomMap[1].score = 10                 — HASHMAP
          → totalScore += 10
          → completedStack.push(1)                — STACK
          → unlockNeighbors(1)                    — GRAPH
            → adjacencyList[1] = {2, 4}
            → roomMap[2].isLocked = false         — HASHMAP
            → roomMap[4].isLocked = false         — HASHMAP
          Return: {correct:true, points:10, unlocked:[2,4]}
```

**5. Timer runs out**
```
Frontend: (every 1 second) fetch("/api/state")
Backend:  remaining = 300 - elapsed - penalties
          remaining <= 0 → gameActive = false
          Return: {gameActive:false}
Frontend: → clearInterval(timer)
          → fetch("/api/history")
Backend:  tempStack = completedStack (copy)
          pop all: Room 5, Room 4, Room 1 (LIFO order)
          Return: {history:[{5,"The Hall",20}, {4,"Workshop",10}, {1,"Lobby",10}]}
```
