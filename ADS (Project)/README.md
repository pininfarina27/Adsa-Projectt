# EscapeX - Escape Room Puzzle Game

A timed escape room game demonstrating 5 core data structures: **Graph, Stack, Queue, HashMap, and Priority Queue**.

Navigate 9 interconnected rooms, solve logic & DSA puzzles, and escape before time runs out!

---

## How to Run (Complete Steps from Scratch)

### Prerequisites

You need only TWO things installed:

1. **A C++ compiler** (g++ version 7 or higher)
   - **Windows**: Install [MSYS2](https://www.msys2.org/), then run: `pacman -S mingw-w64-ucrt-x86_64-gcc`
   - **Linux**: `sudo apt install g++`
   - **Mac**: `xcode-select --install`

2. **A web browser** (Chrome, Firefox, or Edge)

That's it. No other installations needed.

### Step-by-Step

```bash
# Step 1: Open a terminal and navigate to the backend folder
cd "ADS (Project)/backend"

# Step 2: Compile the game server (one command)
# On Windows:
g++ -std=c++17 -o game_server.exe game_server.cpp -lws2_32

# On Linux/Mac:
g++ -std=c++17 -o game_server game_server.cpp -lpthread

# Step 3: Run the server
# On Windows:
.\game_server.exe

# On Linux/Mac:
./game_server

# Step 4: You should see this message:
# ============================================
#   EscapeX Game Server
#   Open http://localhost:8080 in your browser
#   Press Ctrl+C to stop
# ============================================

# Step 5: Open your browser and go to:
#   http://localhost:8080

# Step 6: Play the game!

# Step 7: To stop the server, press Ctrl+C in the terminal
```

### Troubleshooting

| Problem | Solution |
|---|---|
| `g++: command not found` | Install g++ (see Prerequisites above) |
| `httplib.h: No such file` | Make sure you're running the command from the `backend/` folder |
| Port 8080 in use | Kill the other process using port 8080, or change the port in `game_server.cpp` line 739 |
| Browser shows blank page | Make sure the `frontend/` folder exists next to `backend/` with all 3 files |

---

## How to Play

1. **Choose timer** - Select 1 to 5 minutes
2. **Click Start Game** - You begin in Room 1 (The Lobby)
3. **Click a blue room** on the map to enter it
4. **Read the puzzle** and click an answer (A, B, C, or D)
5. **Correct answer** = earn points + unlock adjacent rooms
6. **Wrong answer** = lose 5 seconds (unlimited retries)
7. **Use hints** if stuck (costs 5 points from that room's reward)
8. **Reach Room 9** (The Escape) for a +50 bonus!
9. When the timer runs out, your **final score and history** are shown

### Room Map

```
[1 The Lobby]  ---  [2 The Library]  ---  [3 The Lab]
      |                    |                    |
[4 Workshop]   ---  [5 The Hall]     ---  [6 The Vault]
      |                    |                    |
[7 The Dungeon] --- [8 The Tower]    ---  [9 The Escape]
```

### Scoring

| Action | Points |
|---|---|
| Solve Easy room (1,2,4) | +10 |
| Solve Medium room (3,5,7) | +20 |
| Solve Hard room (6,8,9) | +30 |
| Complete Room 9 (bonus) | +50 |
| Use a hint | -5 from room reward |
| Wrong answer | No point loss, -5 sec only |
| **Max possible score** | **230** |

---

## Project Structure

```
ADS (Project)/
├── backend/
│   ├── game_server.cpp     ← All C++ game logic (5 data structures + HTTP server)
│   ├── questions.h         ← 30 hardcoded questions (logic, math, DSA)
│   ├── httplib.h           ← Single-header HTTP library (external)
│   └── game_server.exe     ← Compiled executable
├── frontend/
│   ├── index.html          ← Game UI (3 screens: start, game, result)
│   ├── style.css           ← Dark theme styling
│   └── game.js             ← Frontend logic (fetch API calls)
├── EscapeX_Project.md      ← Complete project documentation
├── phases.md               ← Development phases
├── memory.md               ← Progress tracker
├── CODE_EXPLANATION.md     ← Line-by-line code explanation
└── README.md               ← This file
```

---

## Data Structures Used

| # | Data Structure | C++ Type | Purpose |
|---|---|---|---|
| 1 | **Graph** | `vector<vector<int>>` | Room connections (adjacency list) |
| 2 | **Stack** | `stack<int>` | Completion history (LIFO) |
| 3 | **Queue** | `queue<Question>` | Question shuffling (FIFO) |
| 4 | **HashMap** | `unordered_map<int, Room>` | Room data storage (O(1)) |
| 5 | **Priority Queue** | `priority_queue<Question>` | Dynamic difficulty (hard first) |

---

## Technology

- **C++17** — backend game logic
- **httplib.h** — single-header HTTP server (no install needed)
- **HTML/CSS/JS** — frontend (no frameworks)
- **Compile** — single g++ command, no build tools

---

*Advanced Data Structures & Algorithms Project*
