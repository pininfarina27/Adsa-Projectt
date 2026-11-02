# 🧠 EscapeX — Memory / Progress Tracker

> This file tracks what has been completed, what's in progress, and what's next.
> Updated after every phase or significant milestone.

---

## Project Summary

| Item | Value |
|---|---|
| **Project** | EscapeX — Escape Room Puzzle Game |
| **Language** | C++ (backend) + HTML/CSS/JS (frontend) |
| **Data Structures** | Graph, Stack, Queue, HashMap, Priority Queue |
| **Rooms** | 9 rooms in a 3×3 graph |
| **Timer** | Dynamic — player chooses 1-5 minutes |
| **Wrong Answer Penalty** | -5 seconds per wrong answer |
| **Retries** | Unlimited |
| **Questions** | Logic, Math Patterns, General Knowledge, Basic DSA |
| **HTTP Library** | httplib.h (single-header, no install) |

---

## Phase Progress

| Phase | Status | Started | Completed | Notes |
|---|---|---|---|---|
| Phase 0: Project Setup | ✅ DONE | 2026-11-02 | 2026-11-02 | Folder structure created, httplib.h downloaded, server compiles and runs |
| Phase 1: Graph + HashMap | ✅ DONE | 2026-11-02 | 2026-11-02 | Adjacency list + room map working, all APIs tested |
| Phase 2: Queue + Stack + PQ | ✅ DONE | 2026-11-02 | 2026-11-02 | All 3 data structures implemented and tested alongside Phase 1 |
| Phase 3: Game Engine | ✅ DONE | 2026-11-02 | 2026-11-02 | Timer, scoring, answer checking, hints, history — all working |
| Phase 4: Frontend (Static) | ✅ DONE | 2026-11-02 | 2026-11-02 | HTML/CSS built: start screen, game screen (3x3 room map + puzzle area), result screen |
| Phase 5: Integration | ✅ DONE | 2026-11-02 | 2026-11-02 | Frontend connected to backend via fetch(), full game playable in browser |
| Phase 6: Polish & Test | ✅ DONE | 2026-11-02 | 2026-11-02 | README.md, CODE_EXPLANATION.md created, full end-to-end test passed |

---

## Files Created

| File | Status | Description |
|---|---|---|
| `EscapeX_Project.md` | ✅ Done | Complete project documentation with rulebook |
| `phases.md` | ✅ Done | Development phases with checklists |
| `memory.md` | ✅ Done | This file — progress tracker |
| `backend/httplib.h` | ✅ Done | Single-header HTTP library (860KB) |
| `backend/game_server.cpp` | ✅ Done | Main C++ backend — all 5 DS + game engine + HTTP server |
| `backend/questions.h` | ✅ Done | 30 questions (10 easy, 10 medium, 10 hard) |
| `backend/game_server.exe` | ✅ Done | Compiled executable (4.76MB) |
| `frontend/index.html` | ✅ Done | Game UI — 3 screens (start, game, result) |
| `frontend/style.css` | ✅ Done | Dark theme, room grid, puzzle cards, animations |
| `frontend/game.js` | ✅ Done | fetch() calls to backend, timer polling, room map updates |
| `README.md` | ✅ Done | Quick start guide with setup instructions |
| `CODE_EXPLANATION.md` | ✅ Done | Detailed line-by-line code walkthrough |

---

## Key Decisions Log

| Date | Decision | Reason |
|---|---|---|
| 2026-11-02 | C++ with httplib.h | Simplest way to serve HTTP from C++, single header file |
| 2026-11-02 | 9 rooms in 3×3 graph | Clean visual layout, manageable scope |
| 2026-11-02 | Dynamic timer (1-5 min) | Player can choose difficulty via time pressure |
| 2026-11-02 | 5-second wrong penalty | Light penalty — enough to add pressure without being punishing |
| 2026-11-02 | Unlimited retries | Player always has a chance, timer is the real constraint |
| 2026-11-02 | Logic/Math/DSA questions | Interesting, educational, fits the CS project theme |

---

## Current Session Notes

### Session: 2026-11-02
- Created project documentation (EscapeX_Project.md)
- Created development phases (phases.md)
- Created memory tracker (this file)
- **Phase 0 COMPLETE**: Folder structure, httplib.h downloaded, server compiles with `g++ -std=c++17`
- **Phase 1 COMPLETE**: Graph (adjacency list) and HashMap (room data) fully implemented
- **Phase 2 COMPLETE**: Queue (question shuffling), Stack (completion history), Priority Queue (difficulty) all working
- **Phase 3 COMPLETE**: Full game engine with timer, scoring, 5-second penalty, hints, answer checking
- **Phase 4 COMPLETE**: Frontend built — index.html (3 screens), style.css (dark theme, grid map), game.js
- **Phase 5 COMPLETE**: Frontend fully connected to backend — all fetch() calls working
- Full integration test passed: start→puzzle→wrong answer→hint→correct→unlock→history
- Frontend files served from C++ server via httplib.h mount_point

#### Frontend Features Built:
- ✅ Start screen with timer dropdown (1-5 minutes)
- ✅ 3x3 room map with visual connectors and state colors (locked=gray, unlocked=blue, completed=green)
- ✅ Puzzle area with question, 4 labeled options (A/B/C/D), hint button
- ✅ Timer with color changes (normal→orange warning at 30s→red danger at 10s + pulse animation)
- ✅ Feedback messages (green for correct, red for wrong)
- ✅ Result screen with score, rooms completed, time spent, and stack history
- ✅ Play Again button reloads the page

#### Test Results (verified via curl/PowerShell):
- ✅ Server starts on localhost:8080
- ✅ Start game with dynamic timer (1-5 min)
- ✅ Room 1 unlocked, rooms 2-9 locked (HashMap)
- ✅ Correct answer: +10 pts, neighbors 2,4 unlocked (Graph adjacency list)
- ✅ Wrong answer: -5 seconds penalty, unlimited retries
- ✅ Hint: returns text, -5 pts from room reward
- ✅ History: Stack returns LIFO order (most recent first)
- ✅ Easy rooms get easy questions from Queue, hard rooms get hard from Priority Queue

### Session: 2026-11-02 (Phase 6 Completion)
- **Phase 6 COMPLETE**: Full end-to-end playthrough testing passed
- Created README.md with from-scratch setup instructions
- Created CODE_EXPLANATION.md with detailed line-by-line code walkthrough
- Updated header comment in game_server.cpp
- Recompiled successfully (exit code 0)
- Ran 18 API tests — all passed:
  - Start game with different timer values (60, 120)
  - Enter unlocked room → question displayed
  - Wrong answer → -5 seconds penalty
  - Correct answer → +10 points, neighbors unlocked via graph
  - Hint → text displayed, -5 points deducted
  - Locked room → error message
  - Completed room → shows completion data
  - History → Stack LIFO order (Room 4 → Room 2 → Room 1)
  - Restart game → clean state (score=0, completed=0, empty history)
  - Frontend files served (HTML 9776B, CSS 10651B, JS 13947B)
- 🎉 **PROJECT COMPLETE** — All 6 phases done, all tests passing

---

## Known Issues / TODOs

- [x] ~~Download httplib.h (Phase 0)~~
- [x] ~~Verify g++ compilation on Windows with -lws2_32~~
- [x] ~~Write 30 interesting questions across 3 difficulty levels~~
- [x] ~~Test CORS headers for frontend-backend communication~~
- [x] ~~Build frontend HTML/CSS (Phase 4)~~
- [x] ~~Connect frontend to backend with fetch() (Phase 5)~~
- [x] ~~Polish, edge cases, final testing (Phase 6)~~
- [x] ~~Write README.md~~
- [x] ~~Write CODE_EXPLANATION.md~~
