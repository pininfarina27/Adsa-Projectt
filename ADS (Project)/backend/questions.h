// ============================================
// EscapeX — Question Bank
// 30 questions: 10 easy, 10 medium, 10 hard
// Topics: Logic, Math Patterns, General Knowledge, Basic DSA
// ============================================

#ifndef QUESTIONS_H
#define QUESTIONS_H

#include <vector>
#include <string>

using namespace std;

// Simple struct to hold question data
struct QuestionData {
    string text;           // The question text
    string hint;           // A helpful hint
    vector<string> options; // 4 answer choices
    int correctOption;     // Index of correct answer (0-3)
    int difficulty;        // 1=easy, 2=medium, 3=hard
    int points;            // 10, 20, or 30
};

// ---- ALL 30 QUESTIONS ----
// Organized by difficulty level

vector<QuestionData> ALL_QUESTIONS = {

    // ========================================
    // EASY QUESTIONS (difficulty=1, points=10)
    // ========================================

    // Q1 — DSA: Array access
    {
        "What is the time complexity of accessing an element in an array by index?",
        "Arrays store elements in contiguous memory — no searching needed",
        {"O(n)", "O(1)", "O(log n)", "O(n^2)"},
        1,  // O(1)
        1, 10
    },

    // Q2 — Logic
    {
        "If you rearrange the letters 'CIFAIPC', you get the name of a(n):",
        "Think about large bodies of water",
        {"City", "Animal", "Ocean", "Country"},
        2,  // Ocean (PACIFIC)
        1, 10
    },

    // Q3 — Math Pattern
    {
        "What comes next in the sequence: 1, 1, 2, 3, 5, 8, ?",
        "Each number is the sum of the two before it",
        {"10", "11", "13", "15"},
        2,  // 13 (Fibonacci)
        1, 10
    },

    // Q4 — DSA: Stack
    {
        "In a Stack, which element is removed first?",
        "Think about stacking plates — which one do you take off first?",
        {"First inserted", "Middle element", "Last inserted", "Random element"},
        2,  // Last inserted (LIFO)
        1, 10
    },

    // Q5 — General Knowledge
    {
        "How many bits are in a single byte?",
        "It's a power of 2, and it's the most fundamental unit in computing",
        {"4", "8", "16", "32"},
        1,  // 8
        1, 10
    },

    // Q6 — Logic
    {
        "A farmer has 17 sheep. All but 9 die. How many sheep are left?",
        "Read the question carefully - 'all but 9'",
        {"8", "17", "9", "0"},
        2,  // 9
        1, 10
    },

    // Q7 — DSA: Queue
    {
        "In a Queue, which element is removed first?",
        "Think of people standing in a line — who gets served first?",
        {"Last inserted", "Middle element", "First inserted", "Random element"},
        2,  // First inserted (FIFO)
        1, 10
    },

    // Q8 — Math
    {
        "What is the sum of all angles in a triangle?",
        "This is a fundamental rule in geometry",
        {"90 degrees", "180 degrees", "270 degrees", "360 degrees"},
        1,  // 180
        1, 10
    },

    // Q9 — DSA: Sorting
    {
        "Which sorting algorithm repeatedly selects the minimum element from the unsorted part?",
        "It 'selects' the smallest each time",
        {"Bubble Sort", "Quick Sort", "Selection Sort", "Merge Sort"},
        2,  // Selection Sort
        1, 10
    },

    // Q10 — Logic
    {
        "What has keys but no locks, space but no room, and you can enter but can't go inside?",
        "You probably used one to read this question",
        {"A map", "A keyboard", "A house", "A car"},
        1,  // Keyboard
        1, 10
    },

    // ==========================================
    // MEDIUM QUESTIONS (difficulty=2, points=20)
    // ==========================================

    // Q11 — Math Pattern
    {
        "What is the next number: 2, 6, 12, 20, 30, ?",
        "Find the differences between consecutive numbers: 4, 6, 8, 10, ...",
        {"40", "42", "36", "38"},
        1,  // 42 (differences increase by 2)
        2, 20
    },

    // Q12 — DSA: Linked List
    {
        "What is the time complexity of inserting at the beginning of a singly linked list?",
        "You just need to update the head pointer",
        {"O(n)", "O(n log n)", "O(1)", "O(log n)"},
        2,  // O(1)
        2, 20
    },

    // Q13 — Logic
    {
        "You have two ropes. Each takes exactly 1 hour to burn, but they burn unevenly. How do you measure 45 minutes?",
        "Light one rope from both ends and the other from one end simultaneously",
        {"Cut one rope in half", "Light both from one end", "Light one from both ends + other from one end", "It's impossible"},
        2,  // Light one from both ends (30 min) + other from one end
        2, 20
    },

    // Q14 — DSA: Binary Search
    {
        "Binary search works on which type of data?",
        "The data must be arranged in order for binary search to work",
        {"Random data", "Sorted data", "Linked list data", "Any data"},
        1,  // Sorted data
        2, 20
    },

    // Q15 — Math
    {
        "A clock shows 3:15. What is the angle between the hour and minute hands?",
        "The minute hand is at 3 (90 degrees). The hour hand has moved slightly past 3",
        {"0 degrees", "7.5 degrees", "15 degrees", "90 degrees"},
        1,  // 7.5 degrees
        2, 20
    },

    // Q16 — DSA: Tree
    {
        "How many leaf nodes does a full binary tree with 7 total nodes have?",
        "In a full binary tree, draw it out: root, 2 children, 4 grandchildren",
        {"3", "4", "7", "2"},
        1,  // 4
        2, 20
    },

    // Q17 — Logic
    {
        "Three boxes are labeled 'Apples', 'Oranges', and 'Mixed'. ALL labels are wrong. You pick one fruit from the 'Mixed' box and it's an apple. What does the 'Oranges' box contain?",
        "Since ALL labels are wrong, the 'Mixed' box has only apples. Now think about the other two...",
        {"Oranges", "Apples", "Mixed", "Empty"},
        2,  // Mixed (since 'Oranges' label is wrong, and 'Apples' box must have oranges)
        2, 20
    },

    // Q18 — DSA: Hashing
    {
        "What is the average time complexity of searching in a hash table?",
        "Hash tables use a hash function for direct access — similar to arrays",
        {"O(n)", "O(log n)", "O(1)", "O(n^2)"},
        2,  // O(1)
        2, 20
    },

    // Q19 — Math Pattern
    {
        "If 1=5, 2=25, 3=125, 4=625, then 5=?",
        "Look at the pattern: 5^1, 5^2, 5^3... but wait, what does 1 equal?",
        {"3125", "1", "5", "25"},
        1,  // 1 (because 1=5 means 5=1)
        2, 20
    },

    // Q20 — DSA: Graph
    {
        "In an undirected graph with 5 vertices, what is the maximum number of edges?",
        "Each vertex can connect to every other vertex. Use n*(n-1)/2",
        {"5", "10", "15", "20"},
        1,  // 10 = 5*4/2
        2, 20
    },

    // ========================================
    // HARD QUESTIONS (difficulty=3, points=30)
    // ========================================

    // Q21 — DSA: Heap
    {
        "In a max-heap with elements [90, 70, 80, 30, 60, 50], what happens after extracting the maximum?",
        "The root is removed, the last element goes to root, then heapify down",
        {"[80, 70, 50, 30, 60]", "[70, 80, 50, 30, 60]", "[80, 60, 70, 30, 50]", "[80, 70, 60, 30, 50]"},
        0,  // [80, 70, 50, 30, 60]
        3, 30
    },

    // Q22 — Logic
    {
        "You have 8 identical-looking balls. One is heavier. Using a balance scale, what is the minimum number of weighings to find the heavy ball?",
        "Divide the balls into groups of 3, 3, and 2",
        {"1", "2", "3", "4"},
        1,  // 2 weighings
        3, 30
    },

    // Q23 — DSA: Complexity
    {
        "What is the time complexity of Merge Sort in the worst case?",
        "Merge Sort always divides in half and merges in linear time",
        {"O(n)", "O(n log n)", "O(n^2)", "O(log n)"},
        1,  // O(n log n)
        3, 30
    },

    // Q24 — Math
    {
        "A snail climbs 3 meters up a wall during the day but slides back 2 meters at night. If the wall is 10 meters, how many days does it take to reach the top?",
        "On the last day, the snail reaches the top during the day and doesn't slide back",
        {"10 days", "8 days", "7 days", "9 days"},
        1,  // 8 days
        3, 30
    },

    // Q25 — DSA: Graph Traversal
    {
        "Which data structure is used in Breadth-First Search (BFS)?",
        "BFS explores level by level — think about which DS processes in order",
        {"Stack", "Queue", "Priority Queue", "Array"},
        1,  // Queue
        3, 30
    },

    // Q26 — Logic
    {
        "In a room are 3 light switches. One controls a bulb in another room. You can only enter the bulb room once. How do you determine which switch controls the bulb?",
        "Think about what information a bulb can give you besides on/off",
        {"Try all switches randomly", "Turn on switch 1, wait, turn off, turn on switch 2, check", "It's impossible", "Use a mirror"},
        1,  // Turn on #1 for 10 min, turn off, turn on #2, go check (hot=1, on=2, off+cold=3)
        3, 30
    },

    // Q27 — DSA: Dynamic Programming
    {
        "How many distinct ways can you climb a staircase of 5 steps if you can take 1 or 2 steps at a time?",
        "Think about it recursively: ways(n) = ways(n-1) + ways(n-2). Start from the base cases",
        {"5", "7", "8", "10"},
        2,  // 8
        3, 30
    },

    // Q28 — DSA: Tree
    {
        "What is the maximum number of nodes in a binary tree of height 3 (root is height 0)?",
        "At each level i, maximum nodes = 2^i. Sum all levels from 0 to 3",
        {"7", "8", "15", "16"},
        2,  // 15 = 2^4 - 1
        3, 30
    },

    // Q29 — Math/Logic
    {
        "Two trains are 200 km apart, heading toward each other at 50 km/h and 70 km/h. A bird flies at 80 km/h between them. How far does the bird fly before the trains meet?",
        "Don't track the bird's path — calculate the time the trains take to meet",
        {"120 km", "133.33 km", "150 km", "160 km"},
        1,  // Trains meet in 200/120 = 5/3 hours. Bird flies 80 * 5/3 = 133.33 km
        3, 30
    },

    // Q30 — DSA: Recursion
    {
        "What is the output of: f(4) where f(n) = n + f(n-1) and f(0) = 0?",
        "Trace it: f(4) = 4+f(3) = 4+3+f(2) = 4+3+2+f(1) = 4+3+2+1+f(0)",
        {"10", "4", "6", "15"},
        0,  // 10 = 4+3+2+1+0
        3, 30
    }
};

#endif
