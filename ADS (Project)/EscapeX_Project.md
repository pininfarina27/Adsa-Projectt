# 🔓 EscapeX — Complete Project Documentation

> **Advanced Data Structures & Algorithms Project**
> A virtual escape room game demonstrating Graph, Stack, Queue, HashMap, and Priority Queue

---

## Table of Contents

1. [Project Overview](#1-project-overview)
2. [What Are We Building?](#2-what-are-we-building)
3. [Why This Project?](#3-why-this-project)
4. [Data Structures Used & Why](#4-data-structures-used--why)
5. [Game Mechanics](#5-game-mechanics)
6. [System Architecture](#6-system-architecture)
7. [Technology Stack & Why](#7-technology-stack--why)
8. [Room Layout & Graph Design](#8-room-layout--graph-design)
9. [API Design](#9-api-design)
10. [Question Bank Design](#10-question-bank-design)
11. [Scoring System](#11-scoring-system)
12. [Strict Coding Rulebook](#12-strict-coding-rulebook)
13. [How to Run](#13-how-to-run)

---

## 1. Project Overview

**Project Name:** EscapeX
**Type:** Interactive Puzzle Game (Web-based)
**Purpose:** Demonstrate practical usage of 5 core data structures through a playable escape room game
**Language:** C++ (Backend) + HTML/CSS/JavaScript (Frontend)
**Complexity Level:** Simple — no advanced OOP, no frameworks, no complex patterns

### One-Line Summary
> A timed escape room game where the player navigates a graph of interconnected rooms, solves puzzles managed by queues and priority queues, tracks progress with a stack, and stores room data in a hash map.

---

## 2. What Are We Building?

EscapeX is a **single-player, timed escape room game** that works in a web browser.

### The Experience
1. The player clicks "Start Game" — they **choose a timer duration** from **1 to 5 minutes**
2. They are placed in **Room 1** (the starting room)
3. The room contains a **puzzle** (multiple-choice question — logic, math, DSA) with a **hint**
4. If they answer **correctly**:
   - They earn points (10/20/30 based on difficulty)
   - The room is marked as completed
   - Connected rooms are unlocked (they can see them on the map)
   - The room ID is pushed onto the completion stack
5. If they answer **incorrectly**:
   - They lose **5 seconds** from the timer
   - They can retry the same room (**unlimited retries**)
6. They choose which unlocked room to enter next
7. As they progress, questions get **harder** (priority queue ensures this)
8. When the timer hits **zero**, the game ends automatically
9. The **final screen** shows:
   - Total score
   - Completion history (popped from the stack — reverse order)
   - A "Play Again" button

### What Makes It a DSA Project
Every single game mechanic maps directly to a data structure. Nothing is there just for show. Each data structure solves a real problem in the game.

---

## 3. Why This Project?

| Traditional DSA Project | EscapeX |
|---|---|
| Print adjacency list to console | Player physically navigates a graph of rooms |
| Push/pop numbers on a stack | Stack tracks real game completion history |
| Enqueue/dequeue abstract items | Queue manages actual question distribution |
| Store key-value pairs | HashMap stores live room state |
| Priority queue sorts numbers | Priority queue controls game difficulty progression |

**The difference:** Instead of demonstrating data structures in isolation with print statements, EscapeX makes them work together in a real interactive system. Each data structure has a clear, visible, necessary role.

---

## 4. Data Structures Used & Why

### 4.1 Graph (Adjacency List) — Room Connections

**What it stores:** Which rooms connect to which other rooms.

**Implementation:**
```cpp
vector<vector<int>> adjList(10); // 9 rooms, index 1-9

// Room 1 connects to Room 2 and Room 4
adjList[1] = {2, 4};
adjList[2] = {1, 3, 5};
// ... and so on
```

**Why a Graph?**
- Rooms are **nodes**, doors between rooms are **edges**
- A player in Room 2 can go to Room 1, 3, or 5 — this is exactly what an adjacency list represents
- We need to quickly find "which rooms can the player go to?" — adjacency list gives O(degree) lookup
- Alternative: A 2D matrix would waste memory for 9 rooms with sparse connections

**Why an Adjacency List (not Adjacency Matrix)?**
- With 9 rooms, a matrix would be 9×9 = 81 cells, mostly zeros
- An adjacency list only stores actual connections — no wasted space
- Simpler to iterate over neighbors

**Where it's used in gameplay:**
- When a player completes a room → check `adjList[roomId]` → unlock those rooms
- When displaying the map → draw edges between connected rooms

---

### 4.2 Stack — Completed Rooms History

**What it stores:** Room IDs in the order they were completed.

**Implementation:**
```cpp
stack<int> completedStack;

// Player completes Room 1
completedStack.push(1);

// Player completes Room 4
completedStack.push(4);

// At game end, pop to show history:
// Room 4 (most recent) → Room 1 (first completed)
```

**Why a Stack?**
- We want to show "what you did most recently first" — this is **LIFO** (Last In, First Out)
- A stack naturally gives reverse-chronological order
- We need to **push** when a room is completed and **pop** at the end to display history
- Alternative: An array with reverse iteration would work, but a stack is the textbook LIFO structure

**Why not just a vector?**
- A vector could work, but the stack enforces the LIFO discipline
- It clearly demonstrates the push/pop paradigm
- The project goal is to **demonstrate** the stack — so we use it where it fits naturally

**Where it's used in gameplay:**
- Push: Every time a room is completed
- Pop: At game end, to build the "Completion History" display
- Score: Sum of all scores from rooms in the stack

---

### 4.3 Queue — Question Management

**What it stores:** Shuffled questions ready to be assigned to rooms.

**Implementation:**
```cpp
queue<Question> questionQueue;

// At game start:
// 1. Take all questions
// 2. Shuffle them randomly
// 3. Push them into the queue

// When a room needs a question:
Question q = questionQueue.front();
questionQueue.pop();
```

**Why a Queue?**
- Questions need to be assigned **one by one, in order, without repetition** — this is **FIFO** (First In, First Out)
- Shuffling first + dequeuing ensures random distribution without repeats
- Once a question is taken from the front, it's gone — no duplicates possible
- Alternative: A random index into an array could work, but requires tracking "used" questions

**Why not a vector with random access?**
- A vector needs extra bookkeeping to track which questions are already used
- A queue naturally consumes items: take from front, it's removed
- Clean, simple, no index management

**Where it's used in gameplay:**
- At game start: All questions shuffled and loaded into the queue
- When initializing a room: Dequeue one question and assign it

---

### 4.4 HashMap — Room Data Storage

**What it stores:** All room information, keyed by room ID.

**Implementation:**
```cpp
unordered_map<int, Room> roomMap;

// Store room data
roomMap[1] = {1, "The Lobby", 1, false, false, -1, 0};

// Access room data in O(1)
Room& room = roomMap[3];
cout << room.name;       // "The Library"
cout << room.isLocked;   // true/false
```

**Why a HashMap?**
- We need to look up room data **by room ID** — O(1) average lookup
- Room IDs are integers (1-9) — perfect keys
- We need fast read AND write (updating `isLocked`, `isCompleted`, `score`)
- Alternative: An array indexed by ID would also be O(1), but HashMap demonstrates the key-value paradigm

**Why not a plain array?**
- An array works for integer keys, but a HashMap generalizes to any key type
- It demonstrates the concept of key-value storage
- It's the standard way to store entity data in real applications

**What each room stores:**
```
Room {
    id:          int       // 1-9
    name:        string    // "The Lobby", "The Lab", etc.
    difficulty:  int       // 1=easy, 2=medium, 3=hard
    isLocked:    bool      // can the player enter?
    isCompleted: bool      // has the player solved it?
    questionId:  int       // which question is assigned
    score:       int       // points earned (0 if not completed)
}
```

---

### 4.5 Priority Queue — Dynamic Difficulty

**What it stores:** Questions ordered by difficulty level.

**Implementation:**
```cpp
// Max-heap: highest difficulty comes out first
priority_queue<Question> difficultyPQ;

// Load harder questions
difficultyPQ.push(hardQuestion1);
difficultyPQ.push(hardQuestion2);

// Get the hardest available question
Question hardest = difficultyPQ.top();
difficultyPQ.pop();
```

**Why a Priority Queue?**
- As the player progresses deeper into the graph, rooms should get harder
- A priority queue automatically gives us the highest-priority (hardest) question
- We use it alongside the regular queue: easy rooms get questions from the queue, hard rooms get questions from the priority queue
- Alternative: Sorting an array works, but priority queue is the dynamic version — we can insert and extract efficiently

**How difficulty progression works:**
- Room difficulty is pre-assigned: Rooms 1,2,4 = easy, Rooms 3,5,7 = medium, Rooms 6,8,9 = hard
- Easy rooms → dequeue from the regular `questionQueue` (random easy question)
- Medium/Hard rooms → pop from the `difficultyPQ` (gets the hardest available)
- This simulates a "boss fight" difficulty curve

---

## 5. Game Mechanics

### 5.1 Timer
- **Duration:** Player chooses at game start — **1 minute, 2 minutes, 3 minutes, 4 minutes, or 5 minutes**
- **Implementation:** Record `time(NULL)` at game start. On each state check: `remaining = chosenTime - (now - startTime) - timePenalty`
- **Wrong answer penalty:** **-5 seconds** per wrong answer
- **Time reaches 0:** Game ends immediately, score is calculated

### 5.2 Scoring
| Event | Points |
|---|---|
| Solve an easy room | +10 |
| Solve a medium room | +20 |
| Solve a hard room | +30 |
| Complete the final room (Room 9) | +50 bonus |
| Use a hint | -5 from that room's reward |
| Wrong answer | No point loss (only time penalty) |

### 5.3 Room Navigation
1. At the start, only Room 1 is unlocked
2. Completing a room unlocks all its graph neighbors
3. The player chooses which unlocked room to enter
4. A room can only be completed once
5. The player can revisit completed rooms but cannot earn points again

### 5.4 Hints
- Each question has one hint
- Clicking "Show Hint" reveals it but reduces that room's reward by 5 points
- Hint is free text (e.g., "Think about the periodic table")

### 5.5 Retry on Wrong Answer
- Wrong answers do NOT lock the room
- The player can retry the same question — **unlimited retries**
- Each wrong attempt costs **5 seconds**
- There is no limit on retries (but the timer is ticking)

---

## 6. System Architecture

```
┌──────────────────────────────────────────────────────┐
│                    BROWSER                           │
│                                                      │
│   index.html ──── style.css                          │
│       │                                              │
│   game.js                                            │
│       │  fetch("http://localhost:8080/api/...")       │
└───────┼──────────────────────────────────────────────┘
        │ HTTP (JSON)
┌───────┼──────────────────────────────────────────────┐
│       ▼           C++ BACKEND                        │
│   httplib.h ──── HTTP Server (port 8080)             │
│       │                                              │
│   game_server.cpp                                    │
│       ├── Graph (adjList)                            │
│       ├── Stack (completedStack)                     │
│       ├── Queue (questionQueue)                      │
│       ├── HashMap (roomMap)                          │
│       └── PriorityQueue (difficultyPQ)               │
│                                                      │
│   questions.h ── Hardcoded question bank             │
└──────────────────────────────────────────────────────┘
```

**Why this architecture?**
- All data structure code stays in C++ (the project requirement)
- The browser provides a visual interface without any complexity
- `httplib.h` is a single file — no installation, no build system
- Communication is simple JSON over HTTP — universally understood

---

## 7. Technology Stack & Why

| Technology | Why |
|---|---|
| **C++17** | Project requirement. Standard, widely available. C++17 for `string_view` and structured bindings (minor conveniences) |
| **httplib.h** | Single-header C++ HTTP server. Zero dependencies. Just `#include` and use. No CMake, no package manager |
| **HTML5** | The simplest way to create a visual interface. No build step |
| **CSS3** | Plain CSS for styling. No frameworks — no Bootstrap, no Tailwind |
| **Vanilla JavaScript** | Plain JS with `fetch()` for API calls. No React, no Vue, no jQuery |
| **g++ compiler** | Available on every platform. One-line compile command |

**What we're NOT using (and why):**
- ❌ CMake/Makefile — overkill for 2 source files
- ❌ JSON library — we build JSON strings manually (5-6 simple concatenations)
- ❌ Database — 30 hardcoded questions in a header file is sufficient
- ❌ WebSocket — simple polling every second is enough for a timer
- ❌ Any CSS/JS framework — adds complexity, learning curve, and build steps

---

## 8. Room Layout & Graph Design

### The Map (9 Rooms)

```
    [1] ─── [2] ─── [3]
     │       │       │
    [4] ─── [5] ─── [6]
     │       │       │
    [7] ─── [8] ─── [9]
```

### Room Details

| Room ID | Name | Difficulty | Connections | Description |
|---|---|---|---|---|
| 1 | The Lobby | Easy | 2, 4 | Starting room. Welcoming, simple puzzle |
| 2 | The Library | Easy | 1, 3, 5 | Books and knowledge |
| 3 | The Lab | Medium | 2, 6 | Science experiments |
| 4 | The Workshop | Easy | 1, 5, 7 | Tools and mechanics |
| 5 | The Hall | Medium | 2, 4, 6, 8 | Central hub, most connections |
| 6 | The Vault | Hard | 3, 5, 9 | Treasures and traps |
| 7 | The Dungeon | Medium | 4, 8 | Dark and challenging |
| 8 | The Tower | Hard | 5, 7, 9 | High above, hard puzzles |
| 9 | The Escape | Hard | 6, 8 | Final room, hardest puzzle + bonus |

### Difficulty Distribution
- **Easy (3 rooms):** 1, 2, 4 — the starting area
- **Medium (3 rooms):** 3, 5, 7 — the middle layer
- **Hard (3 rooms):** 6, 8, 9 — the final push

### Why This Layout?
- The 3×3 grid gives a clean visual representation
- Room 1 (top-left) is an intuitive starting point
- Room 9 (bottom-right) is a clear "exit" / goal
- Room 5 (center) is the hub with 4 connections — strategic choice point
- Difficulty increases naturally as you move away from Room 1
- Multiple paths to Room 9: player can choose strategy (through Lab→Vault or Dungeon→Tower)

---

## 9. API Design

### Endpoints

#### `GET /api/start?time=300`
Starts a new game with the chosen timer duration. Resets all state.

**Query parameter:** `time` — duration in seconds (60, 120, 180, 240, or 300)

**Response:**
```json
{
  "status": "started",
  "timeRemaining": 300,
  "score": 0,
  "rooms": [
    {"id": 1, "name": "The Lobby", "isLocked": false, "isCompleted": false, "difficulty": 1},
    {"id": 2, "name": "The Library", "isLocked": true, "isCompleted": false, "difficulty": 1},
    ...
  ]
}
```

#### `GET /api/state`
Returns current game state (called every second for timer).

#### `GET /api/room/:id`
Returns room details including the puzzle question.

**Response:**
```json
{
  "roomId": 1,
  "name": "The Lobby",
  "question": "What is 2 + 2?",
  "options": ["3", "4", "5", "6"],
  "hint": "Think basic math",
  "hintUsed": false,
  "difficulty": 1,
  "points": 10
}
```

#### `POST /api/answer`
Submit an answer for a room.

**Request:** `{"roomId": 1, "answer": 1}`

**Response (correct):**
```json
{
  "correct": true,
  "pointsEarned": 10,
  "totalScore": 10,
  "timeRemaining": 285,
  "unlockedRooms": [2, 4],
  "message": "Correct! Rooms 2 and 4 are now unlocked!"
}
```

**Response (wrong):**
```json
{
  "correct": false,
  "timePenalty": 5,
  "timeRemaining": 280,
  "message": "Wrong! -5 seconds. Try again!"
}
```

#### `GET /api/hint/:roomId`
Get the hint for a room (costs 5 points from reward).

#### `GET /api/history`
Pop the stack and return completion history.

**Response:**
```json
{
  "history": [
    {"roomId": 5, "name": "The Hall", "points": 20},
    {"roomId": 4, "name": "The Workshop", "points": 10},
    {"roomId": 1, "name": "The Lobby", "points": 10}
  ],
  "totalScore": 40,
  "gameTime": 180
}
```

---

## 10. Question Bank Design

### Categories
Questions are interesting, thought-provoking puzzles across these categories:
- **Math Patterns** (number sequences, series, pattern recognition — NOT basic arithmetic like "2+2")
- **Logic Puzzles** (deductive reasoning, syllogisms, lateral thinking)
- **General Knowledge** (science facts, geography, history — interesting level)
- **Basic DSA** (time complexities, stack/queue behavior, sorting basics, tree traversals)

### Difficulty Levels
| Level | Points | Example |
|---|---|---|
| Easy (1) | 10 | "What is the time complexity of accessing an array element by index?" → O(1) |
| Medium (2) | 20 | "What is the next number in the sequence: 2, 6, 12, 20, ?" → 30 |
| Hard (3) | 30 | "In a min-heap with 7 elements, what is the maximum number of swaps needed to extract the minimum?" → 2 |

### Distribution
- 10 Easy questions
- 10 Medium questions
- 10 Hard questions
- **Total: 30 questions** (only 9 are used per game — plenty of variety)

### Shuffling
At game start, questions are **shuffled randomly** using `random_shuffle()` with `srand(time(NULL))` as the seed. This ensures different question order every playthrough.

---

## 11. Scoring System

### Base Score
```
Total Score = Sum of (room points) for all completed rooms
```

### Modifiers
- **Hint penalty:** Each hint used reduces that room's points by 5
- **Completion bonus:** Completing Room 9 gives +50 bonus
- **Time bonus:** None (keeping it simple)

### Examples
| Scenario | Calculation | Total |
|---|---|---|
| Complete 3 easy rooms | 10 + 10 + 10 | 30 |
| Complete all 9 rooms + final bonus | (10×3) + (20×3) + (30×3) + 50 | 230 |
| Complete 5 rooms, used 2 hints | (10+10+20+20+30) - (5×2) | 80 |

---

## 12. Strict Coding Rulebook

> **These rules are NON-NEGOTIABLE. Every line of code must follow them.**

### C++ Rules

| # | Rule | ❌ Don't | ✅ Do |
|---|---|---|---|
| 1 | **No classes** | `class Game { ... };` | `struct Room { ... };` + free functions |
| 2 | **No inheritance** | `class Dog : public Animal` | Just use `struct` with all fields |
| 3 | **No polymorphism** | `virtual void solve()` | `if (type == "math") solveMath()` |
| 4 | **No smart pointers** | `shared_ptr<Room>` | `Room room;` (value types) |
| 5 | **No raw pointers** | `Room* r = new Room()` | `Room r;` (stack allocation) |
| 6 | **No `new`/`delete`** | `new int[10]` | `vector<int> v(10)` |
| 7 | **No templates** (custom) | `template<typename T>` | Use concrete types |
| 8 | **No operator overloading** (except for PQ) | Complex operator overloads | Only `operator<` for PQ comparison |
| 9 | **No lambda functions** | `auto f = [](int x) { ... }` | Named functions |
| 10 | **No complex STL** | `std::transform`, `std::accumulate` | Simple `for` loops |
| 11 | **Only these STL containers** | `set`, `multimap`, `list` | `vector`, `stack`, `queue`, `priority_queue`, `unordered_map`, `string` |
| 12 | **No `goto`** | `goto retry;` | `while(true)` loop with `break` |
| 13 | **Functions ≤ 30 lines** | 100-line mega function | Split into smaller functions |
| 14 | **Full English variable names** | `int n, q, adj` | `int roomCount, questionId, adjacencyList` |
| 15 | **Comments on every non-obvious line** | Bare code | `// Push completed room onto the stack` |
| 16 | **No nested functions** | Function inside function | All functions at global scope |
| 17 | **No exceptions** | `try/catch/throw` | Return error codes/strings |
| 18 | **No multi-threading** | `std::thread` | Single-threaded (httplib handles this) |

### HTML/CSS/JS Rules

| # | Rule | ❌ Don't | ✅ Do |
|---|---|---|---|
| 1 | **No CSS frameworks** | Bootstrap, Tailwind | Plain CSS |
| 2 | **No JS frameworks** | React, Vue, jQuery | Vanilla JavaScript |
| 3 | **No build tools** | Webpack, Vite, npm | Just open `index.html` |
| 4 | **No TypeScript** | `.ts` files | Plain `.js` files |
| 5 | **No CSS preprocessors** | SASS, LESS | Plain `.css` |
| 6 | **One HTML file** | Multiple pages | Single `index.html` with show/hide sections |
| 7 | **One CSS file** | Split CSS files | Single `style.css` |
| 8 | **One JS file** | Module imports | Single `game.js` |
| 9 | **Only `fetch()` for API** | XMLHttpRequest, Axios | `fetch("url").then(...)` |
| 10 | **DOM manipulation only** | Canvas, WebGL | `document.getElementById()`, `innerHTML` |

### General Rules

| # | Rule |
|---|---|
| 1 | **No external dependencies** except `httplib.h` (single header file) |
| 2 | **No database** — all questions hardcoded in `questions.h` |
| 3 | **No file I/O** — no saving/loading game state |
| 4 | **Compile with one command:** `g++ -std=c++17 -o server game_server.cpp -lpthread` |
| 5 | **Total C++ code ≤ 500 lines** (including comments) |
| 6 | **Total JS code ≤ 300 lines** (including comments) |
| 7 | **Total CSS ≤ 200 lines** |
| 8 | **No code copy-pasted from AI without understanding** |

---

## 13. How to Run

### Prerequisites
- A C++ compiler (`g++` version 7+ or any C++17 compiler)
- A web browser (Chrome, Firefox, Edge)
- That's it. Nothing else to install.

### Steps
```bash
# 1. Navigate to the backend folder
cd "ADS (Project)/backend"

# 2. Compile (one command)
g++ -std=c++17 -o game_server game_server.cpp -lpthread

# 3. Run the server
./game_server
# Output: "EscapeX server running on http://localhost:8080"

# 4. Open browser
# Go to http://localhost:8080

# 5. Play the game!
```

### On Windows (if using MinGW)
```cmd
g++ -std=c++17 -o game_server.exe game_server.cpp -lws2_32
game_server.exe
```

---

*Last updated: November 2, 2026*
*Project by: [Your Name]*
*Course: Advanced Data Structures & Algorithms*
