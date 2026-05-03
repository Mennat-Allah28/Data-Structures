// ================================================================
//  SNAKE GAME
//  Compile: g++ -std=c++17 -O2 -o snake snake_game.cpp
//  Run:     ./snake
//  Controls: Arrow Keys / WASD  |  P=Pause  R=Restart  Q=Quit
//
//  Data Structures Used:
//    - Array        : Board grid rendering
//    - Linked List  : Snake body segments
//    - Queue        : Direction input buffer
//    - Stack        : Score history per session
//    - BST          : Leaderboard (sorted by score)
// ================================================================

#ifdef _WIN32
  #include <windows.h>
  void SetConsoleUTF8() {
      SetConsoleOutputCP(CP_UTF8);
      SetConsoleCP(CP_UTF8);
  }
#else
  void SetConsoleUTF8() {}
#endif

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <chrono>
#include <stdexcept>

// ================================================================
//  SECTION 1: DATA STRUCTURES
// ================================================================

// ----------------------------------------------------------------
//  1. LINKED LIST - Snake Body Segments
// ----------------------------------------------------------------
struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Point& o) const { return x == o.x && y == o.y; }
};

struct SnakeNode {
    Point pos;
    SnakeNode* next;
    SnakeNode* prev;
    SnakeNode(Point p) : pos(p), next(nullptr), prev(nullptr) {}
};

class SnakeLinkedList {
public:
    SnakeNode* head;
    SnakeNode* tail;
    int length;

    SnakeLinkedList() : head(nullptr), tail(nullptr), length(0) {}

    void addFront(Point p) {
        SnakeNode* node = new SnakeNode(p);
        if (!head) { head = tail = node; }
        else { node->next = head; head->prev = node; head = node; }
        length++;
    }

    void removeTail() {
        if (!tail) return;
        if (tail == head) { delete tail; head = tail = nullptr; }
        else {
            SnakeNode* prev = tail->prev;
            delete tail;
            tail = prev;
            tail->next = nullptr;
        }
        length--;
    }

    bool containsPoint(Point p) const {
        SnakeNode* cur = head;
        while (cur) { if (cur->pos == p) return true; cur = cur->next; }
        return false;
    }

    // Returns all positions as an ARRAY (vector)
    std::vector<Point> getAllPositions() const {
        std::vector<Point> pts;
        SnakeNode* cur = head;
        while (cur) { pts.push_back(cur->pos); cur = cur->next; }
        return pts;
    }

    void reset() {
        SnakeNode* cur = head;
        while (cur) { SnakeNode* next = cur->next; delete cur; cur = next; }
        head = tail = nullptr;
        length = 0;
    }

    ~SnakeLinkedList() { reset(); }
};

// ----------------------------------------------------------------
//  2. QUEUE - Movement Direction Buffer
// ----------------------------------------------------------------
template<typename T>
class DirectionQueue {
    struct QNode { T data; QNode* next; QNode(T d) : data(d), next(nullptr) {} };
    QNode* front;
    QNode* back;
    int sz;
public:
    DirectionQueue() : front(nullptr), back(nullptr), sz(0) {}

    void enqueue(T val) {
        if (sz >= 3) return;
        QNode* node = new QNode(val);
        if (!back) { front = back = node; }
        else { back->next = node; back = node; }
        sz++;
    }

    T dequeue() {
        if (!front) throw std::runtime_error("Queue empty");
        T val = front->data;
        QNode* tmp = front;
        front = front->next;
        if (!front) back = nullptr;
        delete tmp;
        sz--;
        return val;
    }

    bool isEmpty() const { return sz == 0; }

    ~DirectionQueue() { while (!isEmpty()) dequeue(); }
};

// ----------------------------------------------------------------
//  3. STACK - Score History (tracks all scores in session)
// ----------------------------------------------------------------
template<typename T>
class GameStack {
    struct SNode { T data; SNode* next; SNode(T d) : data(d), next(nullptr) {} };
    SNode* top_node;
    int sz;
public:
    GameStack() : top_node(nullptr), sz(0) {}

    void push(T val) {
        SNode* node = new SNode(val);
        node->next = top_node;
        top_node = node;
        sz++;
        // Keep only last 50 entries
        if (sz > 50) {
            SNode* cur = top_node;
            for (int i = 0; i < 48; i++) cur = cur->next;
            SNode* tmp = cur->next;
            cur->next = nullptr;
            while (tmp) { SNode* nx = tmp->next; delete tmp; tmp = nx; sz--; }
        }
    }

    bool isEmpty() const { return sz == 0; }

    ~GameStack() {
        while (top_node) { SNode* tmp = top_node; top_node = top_node->next; delete tmp; }
    }
};

// ----------------------------------------------------------------
//  4. BST - Leaderboard (sorted by score, all players)
// ----------------------------------------------------------------
struct ScoreEntry {
    int score;
    std::string name;
    ScoreEntry() : score(0), name("") {}
    ScoreEntry(int s, std::string n) : score(s), name(n) {}
    bool operator>(const ScoreEntry& o) const { return score > o.score; }
};

class LeaderboardBST {
    struct BSTNode {
        ScoreEntry data;
        BSTNode* left;
        BSTNode* right;
        BSTNode(ScoreEntry d) : data(d), left(nullptr), right(nullptr) {}
    };
    BSTNode* root;

    BSTNode* insert(BSTNode* node, ScoreEntry entry) {
        if (!node) return new BSTNode(entry);
        if (entry > node->data) node->right = insert(node->right, entry);
        else node->left = insert(node->left, entry);
        return node;
    }

    void inorderDesc(BSTNode* node, std::vector<ScoreEntry>& result, int limit) const {
        if (!node || (int)result.size() >= limit) return;
        inorderDesc(node->right, result, limit);
        if ((int)result.size() < limit) result.push_back(node->data);
        inorderDesc(node->left, result, limit);
    }

    void destroy(BSTNode* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    LeaderboardBST() : root(nullptr) {}
    void addScore(int score, std::string name) { root = insert(root, ScoreEntry(score, name)); }
    std::vector<ScoreEntry> getTopScores(int n = 5) const {
        std::vector<ScoreEntry> result;
        inorderDesc(root, result, n);
        return result;
    }
    bool hasScores() const { return root != nullptr; }
    ~LeaderboardBST() { destroy(root); }
};

// ================================================================
//  SECTION 2: GAME CONSTANTS & STATE
// ================================================================

const int BOARD_W = 40;
const int BOARD_H = 30;

// 5. ARRAY - 2D board grid used for rendering
//    int board[BOARD_H][BOARD_W] declared inside drawGame()

enum Direction { UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3 };
enum GameState { MENU, NAME_INPUT, PLAYING, PAUSED, GAME_OVER, LEADERBOARD };

struct Food {
    Point pos;
    int type;
    int value;
    bool active;
    Food() : type(0), value(1), active(false) {}
};

struct Obstacle { Point pos; };

// ================================================================
//  SECTION 3: GAME ENGINE
// ================================================================

class SnakeGame {
public:
    SnakeLinkedList           snake;
    DirectionQueue<Direction> dirQueue;
    GameStack<int>            scoreHistory;
    LeaderboardBST            leaderboard;

    GameState   state;
    Direction   currentDir;
    Food        food;
    Food        bonusFood;
    int         score;
    int         level;
    int         foodEaten;
    int         tickInterval;
    std::string playerName;
    std::string nameBuffer;   // used while typing name
    std::vector<Obstacle> obstacles;

    SnakeGame() {
        srand((unsigned)time(nullptr));
        state        = MENU;
        currentDir   = RIGHT;
        score        = 0;
        level        = 1;
        foodEaten    = 0;
        tickInterval = 200;
        playerName   = "";
        nameBuffer   = "";
    }

    void init() {
        snake.reset();
        int mx = BOARD_W / 2;
        int my = BOARD_H / 2;
        snake.addFront(Point(mx,   my));
        snake.addFront(Point(mx+1, my));
        snake.addFront(Point(mx+2, my));

        currentDir   = RIGHT;
        score        = 0;
        foodEaten    = 0;
        tickInterval = 200 - (level - 1) * 15;
        obstacles.clear();
        bonusFood.active = false;

        placeObstacles();
        spawnFood();
        state = PLAYING;
    }

    void placeObstacles() {
        if (level < 3) return;          // obstacles start from level 3
        int count = (level - 2) * 3;   // level3=3, level4=6, level5=9, ...
        for (int i = 0; i < count; i++) {
            Obstacle obs;
            obs.pos = randomEmptyCell();
            if (!(obs.pos == Point(-1,-1)))
                obstacles.push_back(obs);
        }
    }

    // addding new obstacles at each new level up during playing
    void addObstaclesForLevel() {
        if (level < 3) return;
        int toAdd = 3;  //3 new obstacles with each new level
        for (int i = 0; i < toAdd; i++) {
            Obstacle obs;
            obs.pos = randomEmptyCell();
            if (!(obs.pos == Point(-1,-1)))
                obstacles.push_back(obs);
        }
    }

    Point randomEmptyCell() {
        for (int tries = 0; tries < 200; tries++) {
            int x = 1 + rand() % (BOARD_W - 2);
            int y = 1 + rand() % (BOARD_H - 2);
            Point p(x, y);
            if (snake.containsPoint(p)) continue;
            if (food.active && food.pos == p) continue;
            bool obs = false;
            for (auto& o : obstacles) if (o.pos == p) { obs = true; break; }
            if (obs) continue;
            return p;
        }
        return Point(-1, -1);
    }

    void spawnFood() {
        food.pos    = randomEmptyCell();
        food.type   = 0;
        food.value  = 1;
        food.active = true;
        if (rand() % 4 == 0) {
            bonusFood.pos    = randomEmptyCell();
            bonusFood.type   = (rand() % 2) + 1;
            bonusFood.value  = bonusFood.type == 1 ? 3 : 5;
            bonusFood.active = true;
        }
    }

    bool update() {
        if (state != PLAYING) return true;

        Direction nextDir = currentDir;
        if (!dirQueue.isEmpty()) {
            Direction d = dirQueue.dequeue();
            if (!((d == UP    && currentDir == DOWN)  ||
                  (d == DOWN  && currentDir == UP)    ||
                  (d == LEFT  && currentDir == RIGHT) ||
                  (d == RIGHT && currentDir == LEFT)))
                nextDir = d;
        }
        currentDir = nextDir;

        Point head = snake.head->pos;
        switch (currentDir) {
            case UP:    head.y--; break;
            case DOWN:  head.y++; break;
            case LEFT:  head.x--; break;
            case RIGHT: head.x++; break;
        }

        if (head.x <= 0 || head.x >= BOARD_W-1 ||
            head.y <= 0 || head.y >= BOARD_H-1) { endGame(); return false; }
        if (snake.containsPoint(head))            { endGame(); return false; }
        for (auto& o : obstacles)
            if (o.pos == head)                    { endGame(); return false; }

        snake.addFront(head);

        if (food.active && head == food.pos) {
            score += food.value * level;
            scoreHistory.push(score);
            foodEaten++;
            food.active = false;
            checkLevelUp();
            spawnFood();
        } else if (bonusFood.active && head == bonusFood.pos) {
            score += bonusFood.value * level;
            scoreHistory.push(score);
            foodEaten++;
            bonusFood.active = false;
            checkLevelUp();
        } else {
            snake.removeTail();
        }
        return true;
    }

    void checkLevelUp() {
        int nl = 1 + (foodEaten / 5);
        if (nl > level && nl <= 10) {
            level = nl;
            tickInterval = std::max(60, 200 - (level - 1) * 15);
            addObstaclesForLevel();  // اضافة obstacles عند كل level up
        }
    }

    void queueDirection(Direction d) { dirQueue.enqueue(d); }

    void togglePause() {
        if (state == PLAYING) state = PAUSED;
        else if (state == PAUSED) state = PLAYING;
    }

    void endGame() {
        state = GAME_OVER;
        // Save score with player name into BST leaderboard
        if (!playerName.empty())
            leaderboard.addScore(score, playerName);
        else
            leaderboard.addScore(score, "UNKNOWN");
    }
};

// ================================================================
//  SECTION 4: ANSI / RENDERER
// ================================================================

#define RESET             "\033[0m"
#define BOLD              "\033[1m"
#define DIM               "\033[2m"
#define BLINK             "\033[5m"
#define FG_RED            "\033[31m"
#define FG_GREEN          "\033[32m"
#define FG_CYAN           "\033[36m"
#define FG_WHITE          "\033[37m"
#define FG_BRIGHT_GREEN   "\033[92m"
#define FG_BRIGHT_YELLOW  "\033[93m"
#define FG_BRIGHT_MAGENTA "\033[95m"
#define FG_BRIGHT_CYAN    "\033[96m"
#define FG_BRIGHT_WHITE   "\033[97m"
#define FG_ORANGE         "\033[38;5;208m"
#define FG_LIME           "\033[38;5;154m"
#define BG_DARK           "\033[48;5;17m"
#define CLEAR_SCREEN      "\033[2J\033[H"
#define HIDE_CURSOR       "\033[?25l"
#define SHOW_CURSOR       "\033[?25h"

class Renderer {
    bool firstFrame;
public:
    Renderer() : firstFrame(true) { std::cout << HIDE_CURSOR; }
    ~Renderer() { std::cout << SHOW_CURSOR << RESET; }

    void clearScreen() { std::cout << CLEAR_SCREEN; }
    void moveTo(int row, int col) { std::cout << "\033[" << row << ";" << col << "H"; }

    // ---- NAME INPUT SCREEN ----
    void drawNameInput(const std::string& nameBuffer) {
        clearScreen();
        int row = 5;
        auto ln = [&](const std::string& s) { moveTo(row++, 8); std::cout << s; };

        ln(std::string(FG_BRIGHT_CYAN) + BOLD + "╔══════════════════════════════════════════╗" + RESET);
        ln(std::string(FG_BRIGHT_CYAN) + "║" + FG_BRIGHT_YELLOW + BOLD +
           "          🐍  SNAKE GAME  🐍          " + FG_BRIGHT_CYAN + "    ║" + RESET);
        ln(std::string(FG_BRIGHT_CYAN) + "╚══════════════════════════════════════════╝" + RESET);
        row++;
        ln(std::string(FG_BRIGHT_WHITE) + BOLD + "  Enter your name:" + RESET);
        row++;

        // Display typed name with cursor
        std::string display = "  > " + nameBuffer + "_";
        ln(std::string(FG_BRIGHT_YELLOW) + BOLD + display + RESET);
        row++;
        ln(std::string(FG_WHITE) + DIM + "  (Press ENTER to confirm, max 10 chars)" + RESET);
        std::cout.flush();
    }

    // ---- GAME BOARD ----
    void drawGame(const SnakeGame& game) {
        if (firstFrame) { clearScreen(); firstFrame = false; }

        // 5. ARRAY - 2D board used for rendering
        int board[BOARD_H][BOARD_W];
        memset(board, 0, sizeof(board));

        // Walls
        for (int x = 0; x < BOARD_W; x++) { board[0][x] = 1; board[BOARD_H-1][x] = 1; }
        for (int y = 0; y < BOARD_H; y++) { board[y][0] = 1; board[y][BOARD_W-1] = 1; }

        // Obstacles
        for (auto& o : game.obstacles)
            if (o.pos.y>=0 && o.pos.y<BOARD_H && o.pos.x>=0 && o.pos.x<BOARD_W)
                board[o.pos.y][o.pos.x] = 8;

        // Food
        if (game.food.active)
            board[game.food.pos.y][game.food.pos.x] = 5;
        if (game.bonusFood.active)
            board[game.bonusFood.pos.y][game.bonusFood.pos.x] =
                (game.bonusFood.type == 1) ? 6 : 7;

        // Snake (from linked list)
        auto positions = game.snake.getAllPositions();
        for (int i = (int)positions.size()-1; i >= 0; i--) {
            int x = positions[i].x, y = positions[i].y;
            if (y<0||y>=BOARD_H||x<0||x>=BOARD_W) continue;
            if (i == 0) board[y][x] = 2;
            else if (i == (int)positions.size()-1) board[y][x] = 4;
            else board[y][x] = 3;
        }

        // Render board from array
        moveTo(1, 1);
        for (int y = 0; y < BOARD_H; y++) {
            for (int x = 0; x < BOARD_W; x++) {
                switch (board[y][x]) {
                    case 1:
                        if      (y==0 && x==0)                  std::cout << FG_CYAN << "╔" << RESET;
                        else if (y==0 && x==BOARD_W-1)          std::cout << FG_CYAN << "╗" << RESET;
                        else if (y==BOARD_H-1 && x==0)          std::cout << FG_CYAN << "╚" << RESET;
                        else if (y==BOARD_H-1 && x==BOARD_W-1)  std::cout << FG_CYAN << "╝" << RESET;
                        else if (y==0 || y==BOARD_H-1)          std::cout << FG_CYAN << "══" << RESET;
                        else                                     std::cout << FG_CYAN << "║ " << RESET;
                        break;
                    case 2: std::cout << "😊"; break;
                    case 3: std::cout << "🟩"; break;
                    case 4: std::cout << "🔵"; break;
                    case 5: std::cout << "🍎"; break;
                    case 6: std::cout << "🍊"; break;
                    case 7: std::cout << "⭐"; break;
                    case 8: std::cout << "🧱"; break;
                    default: std::cout << BG_DARK << "  " << RESET; break;
                }
            }
            std::cout << "\n";
        }

        drawSidePanel(game);
        std::cout.flush();
    }

    void drawSidePanel(const SnakeGame& game) {
        int col = BOARD_W * 2 + 4;
        auto at = [&](int row) { moveTo(row, col); };

        at(1);  std::cout << FG_BRIGHT_CYAN << BOLD << "╔════════════════════╗" << RESET;
        at(2);  std::cout << FG_BRIGHT_CYAN << "║" << FG_BRIGHT_YELLOW << BOLD
                          << "  🐍  SNAKE  🐍   " << FG_BRIGHT_CYAN << "  ║" << RESET;
        at(3);  std::cout << FG_BRIGHT_CYAN << "╚════════════════════╝" << RESET;

        at(4);  std::cout << FG_BRIGHT_CYAN << "  Player: "
                          << FG_BRIGHT_YELLOW << BOLD << game.playerName << RESET;

        at(6);  std::cout << FG_BRIGHT_WHITE << BOLD << "  SCORE" << RESET;
        at(7);  std::cout << FG_BRIGHT_YELLOW << BOLD << "  " << std::setw(8) << game.score << RESET;

        at(9);  std::cout << FG_BRIGHT_WHITE << BOLD << "  LEVEL" << RESET;
        at(10); std::cout << FG_LIME << BOLD << "  ";
        for (int i = 0; i < game.level; i++) std::cout << "★";
        for (int i = game.level; i < 10; i++) std::cout << "☆";
        std::cout << RESET;

        at(12); std::cout << FG_BRIGHT_WHITE << BOLD << "  LENGTH" << RESET;
        at(13); std::cout << FG_BRIGHT_GREEN << BOLD << "  " << game.snake.length << " segs" << RESET;

        at(15); std::cout << FG_BRIGHT_WHITE << BOLD << "  FOOD EATEN" << RESET;
        at(16); std::cout << FG_BRIGHT_MAGENTA << "  " << game.foodEaten << RESET;

        at(18); std::cout << FG_BRIGHT_WHITE << BOLD << "  SPEED" << RESET;
        at(19); std::cout << FG_ORANGE << "  " << (1000/game.tickInterval) << " mv/s" << RESET;

        at(21); std::cout << FG_BRIGHT_CYAN << "────────────────────" << RESET;
        at(22); std::cout << FG_WHITE << "  [WASD/Arrows] Move" << RESET;
        at(23); std::cout << FG_WHITE << "  [P] Pause"          << RESET;
        at(24); std::cout << FG_WHITE << "  [R] Restart"        << RESET;
        at(25); std::cout << FG_WHITE << "  [L] Leaderboard"    << RESET;
        at(26); std::cout << FG_WHITE << "  [Q] Quit"           << RESET;

        at(28); std::cout << FG_BRIGHT_CYAN << "────────────────────" << RESET;
        at(29); std::cout << "🍎 Normal  +" << 1*game.level;
        at(30); std::cout << "🍊 Bonus   +" << 3*game.level;
        at(31); std::cout << "⭐ Super   +" << 5*game.level;
        at(32); std::cout << "🧱 Obstacle";

        if (game.state == PAUSED) {
            at(34); std::cout << FG_BRIGHT_YELLOW << BOLD << BLINK
                              << "  ⏸  PAUSED  ⏸" << RESET;
        }
    }

    // ---- MAIN MENU ----
    void drawMenu() {
        clearScreen();
        int row = 3;
        auto ln = [&](const std::string& s) { moveTo(row++, 8); std::cout << s; };

        ln(std::string(FG_BRIGHT_CYAN) + BOLD + "╔══════════════════════════════════════╗" + RESET);
        ln(std::string(FG_BRIGHT_CYAN) + "║" + FG_BRIGHT_YELLOW + BOLD +
           "          🐍  SNAKE GAME  🐍          " + FG_BRIGHT_CYAN + "║" + RESET);
        ln(std::string(FG_BRIGHT_CYAN) + "╚══════════════════════════════════════╝" + RESET);
        row++;
        ln(std::string(FG_BRIGHT_GREEN) + BOLD + "      ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░  " + RESET);
        ln(std::string(FG_BRIGHT_GREEN) + BOLD + "   🐍 ░  Eat the food, grow longer  ░  " + RESET);
        ln(std::string(FG_BRIGHT_GREEN) + BOLD + "      ░  Don't hit the walls/self!  ░  " + RESET);
        ln(std::string(FG_BRIGHT_GREEN) + BOLD + "      ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░   " + RESET);
        row++;
        ln(std::string(FG_BRIGHT_WHITE) + BOLD + "  ┌─────────────────────────────────────┐  " + RESET);
        ln(std::string(FG_BRIGHT_WHITE) + "  │" + FG_BRIGHT_YELLOW + BOLD +
           "   [ENTER]  Play (enter name first)  " + FG_BRIGHT_WHITE + "│  " + RESET);
        ln(std::string(FG_BRIGHT_WHITE) + "  │" + FG_BRIGHT_CYAN +
           "   [L]      Leaderboard              " + FG_BRIGHT_WHITE + "│  " + RESET);
        ln(std::string(FG_BRIGHT_WHITE) + "  │" + FG_RED +
           "   [Q]      Quit                     " + FG_BRIGHT_WHITE + "│  " + RESET);
        ln(std::string(FG_BRIGHT_WHITE) + "  └─────────────────────────────────────┘  " + RESET);
        row++;
        ln(std::string(FG_BRIGHT_MAGENTA) + "       🍎 +1pt   🍊 +3pts   ⭐ +5pts" + RESET);
        row++;
        ln(std::string(DIM) + FG_WHITE + "  Higher levels = faster snake + obstacles!" + RESET);
        std::cout.flush();
    }

    // ---- GAME OVER ----
    void drawGameOver(const SnakeGame& game) {
        clearScreen();
        int row = 4;
        auto ln = [&](const std::string& s) { moveTo(row++, 10); std::cout << s; };

        ln(std::string(FG_RED) + BOLD + "╔══════════════════════════════════╗" + RESET);
        ln(std::string(FG_RED) + BOLD + "║      💀  GAME  OVER  💀          ║" + RESET);
        ln(std::string(FG_RED) + BOLD + "╚══════════════════════════════════╝" + RESET);
        row++;
        ln(std::string(FG_BRIGHT_WHITE) + BOLD + "  Player:     " + FG_BRIGHT_YELLOW + BOLD + game.playerName + RESET);
        ln(std::string(FG_BRIGHT_WHITE) + BOLD + "  Score:      " + FG_BRIGHT_YELLOW + BOLD + std::to_string(game.score)       + RESET);
        ln(std::string(FG_BRIGHT_WHITE) + BOLD + "  Level:      " + FG_LIME          + BOLD + std::to_string(game.level)       + RESET);
        ln(std::string(FG_BRIGHT_WHITE) + BOLD + "  Length:     " + FG_BRIGHT_GREEN  + BOLD + std::to_string(game.snake.length) + RESET);
        ln(std::string(FG_BRIGHT_WHITE) + BOLD + "  Food Eaten: " + FG_ORANGE        + BOLD + std::to_string(game.foodEaten)   + RESET);
        row++;
        ln(std::string(FG_BRIGHT_CYAN) + BOLD + "  Top Scores (this session):" + RESET);

        auto tops = game.leaderboard.getTopScores(5);
        int rank = 1;
        for (auto& e : tops) {
            std::string medal = (rank==1)?"🥇":(rank==2)?"🥈":(rank==3)?"🥉":"  ";
            std::string namePadded = e.name; while ((int)namePadded.size() < 12) namePadded += ' ';
            ln(medal + std::string(FG_BRIGHT_WHITE) + " " + std::to_string(rank) + ". " +
               namePadded + std::string(FG_BRIGHT_YELLOW) + std::to_string(e.score) + " pts" + RESET);
            rank++;
        }
        row++;
        ln(std::string(FG_BRIGHT_GREEN) + BOLD + "  [R] Play Again  [L] Leaderboard  [Q] Quit" + RESET);
        std::cout.flush();
    }

    // ---- LEADERBOARD ----
    void drawLeaderboard(const SnakeGame& game) {
        clearScreen();
        int row = 3;
        auto ln = [&](const std::string& s) { moveTo(row++, 10); std::cout << s; };

        ln(std::string(FG_BRIGHT_YELLOW) + BOLD + "╔══════════════════════════════════════╗" + RESET);
        ln(std::string(FG_BRIGHT_YELLOW) + "║" + FG_BRIGHT_WHITE + BOLD +
           "      🏆  SESSION LEADERBOARD  🏆     " + FG_BRIGHT_YELLOW + "║" + RESET);
        ln(std::string(FG_BRIGHT_YELLOW) + BOLD + "╠══════════════════════════════════════╣" + RESET);
        row++;

        auto tops = game.leaderboard.getTopScores(10);
        if (tops.empty()) {
            ln(std::string(FG_WHITE) + DIM + "  No scores yet. Play a game first!" + RESET);
        } else {
            // Header
            ln(std::string(FG_BRIGHT_CYAN) + "  Rank  Name            Score" + RESET);
            ln(std::string(FG_BRIGHT_CYAN) + "  ────  ──────────────  ─────" + RESET);
            int rank = 1;
            for (auto& e : tops) {
                std::string medal, color;
                if (rank==1){ medal="🥇"; color=FG_BRIGHT_YELLOW; }
                else if(rank==2){ medal="🥈"; color="\033[37m"; }
                else if(rank==3){ medal="🥉"; color=FG_ORANGE; }
                else { medal="  "; color=FG_WHITE; }

                std::ostringstream line;
                line << "  " << medal << " "
                     << color << std::left << std::setw(14) << e.name
                     << "  " << std::right << std::setw(5) << e.score
                     << " pts" << RESET;
                ln(line.str());
                rank++;
            }
        }
        row++;
        ln(std::string(FG_BRIGHT_CYAN) + "  [ENTER] Play   [Q] Back to Menu" + RESET);
        std::cout.flush();
    }
};

// ================================================================
//  SECTION 5: INPUT HANDLER
// ================================================================

#ifdef _WIN32
  #include <conio.h>
  class InputHandler {
  public:
      static const int KEY_UP=1000, KEY_DOWN=1001, KEY_LEFT=1002, KEY_RIGHT=1003;
      InputHandler() {}
      ~InputHandler() {}
      int getKey() {
          if (!_kbhit()) return 0;
          int ch = _getch();
          if (ch == 0 || ch == 224) {
              int ch2 = _getch();
              if (ch2 == 72) return KEY_UP;
              if (ch2 == 80) return KEY_DOWN;
              if (ch2 == 75) return KEY_LEFT;
              if (ch2 == 77) return KEY_RIGHT;
          }
          return ch;
      }
      void sleepMs(int ms) { Sleep(ms); }
  };
#else
  #include <termios.h>
  #include <unistd.h>
  #include <fcntl.h>
  class InputHandler {
      struct termios orig_termios;
  public:
      static const int KEY_UP=1000, KEY_DOWN=1001, KEY_LEFT=1002, KEY_RIGHT=1003;
      InputHandler() {
          tcgetattr(STDIN_FILENO, &orig_termios);
          struct termios raw = orig_termios;
          raw.c_lflag &= ~(ECHO | ICANON | ISIG);
          raw.c_cc[VMIN] = 0; raw.c_cc[VTIME] = 0;
          tcsetattr(STDIN_FILENO, TCSANOW, &raw);
          int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
          fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
      }
      ~InputHandler() {
          tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
          int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
          fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
      }
      int getKey() {
          char c;
          if (read(STDIN_FILENO, &c, 1) <= 0) return 0;
          if (c == '\033') {
              char seq[3] = {0};
              (void)read(STDIN_FILENO, &seq[0], 1);
              (void)read(STDIN_FILENO, &seq[1], 1);
              if (seq[0]=='[') {
                  if (seq[1]=='A') return KEY_UP;
                  if (seq[1]=='B') return KEY_DOWN;
                  if (seq[1]=='C') return KEY_RIGHT;
                  if (seq[1]=='D') return KEY_LEFT;
              }
              return 27;
          }
          if (c=='w'||c=='W') return KEY_UP;
          if (c=='s'||c=='S') return KEY_DOWN;
          if (c=='a'||c=='A') return KEY_LEFT;
          if (c=='d'||c=='D') return KEY_RIGHT;
          return (unsigned char)c;
      }
      void sleepMs(int ms) {
          struct timespec ts;
          ts.tv_sec = ms/1000; ts.tv_nsec = (ms%1000)*1000000L;
          nanosleep(&ts, nullptr);
      }
  };
#endif

// ================================================================
//  SECTION 6: MAIN GAME LOOP
// ================================================================

int main() {
    SetConsoleUTF8();
    SnakeGame    game;
    Renderer     renderer;
    InputHandler input;

    renderer.drawMenu();

    using Clock = std::chrono::steady_clock;
    auto lastTick = Clock::now();
    bool running  = true;

    while (running) {
        int key = input.getKey();

        if (key != 0) {
            switch (game.state) {

                // ---- MAIN MENU ----
                case MENU:
                    if (key=='\n' || key=='\r' || key==' ') {
                        // Go to name input before starting
                        game.nameBuffer = "";
                        game.state = NAME_INPUT;
                        renderer.drawNameInput(game.nameBuffer);
                    }
                    else if (key=='l' || key=='L') {
                        game.state = LEADERBOARD;
                        renderer.drawLeaderboard(game);
                    }
                    else if (key=='q' || key=='Q') {
                        running = false;
                    }
                    break;

                // ---- NAME INPUT ----
                case NAME_INPUT:
                    if (key == '\n' || key == '\r') {
                        // Confirm name and start game
                        if (game.nameBuffer.empty()) game.nameBuffer = "PLAYER";
                        game.playerName = game.nameBuffer;
                        game.level = 1;
                        game.init();
                        lastTick = Clock::now();
                    }
                    else if ((key == 127 || key == 8) && !game.nameBuffer.empty()) {
                        // Backspace
                        game.nameBuffer.pop_back();
                        renderer.drawNameInput(game.nameBuffer);
                    }
                    else if (key >= 32 && key <= 126 && (int)game.nameBuffer.size() < 10) {
                        // Printable character
                        game.nameBuffer += (char)key;
                        renderer.drawNameInput(game.nameBuffer);
                    }
                    else if (key == 27 || key == 'q' || key == 'Q') {
                        game.state = MENU;
                        renderer.drawMenu();
                    }
                    break;

                // ---- PLAYING ----
                case PLAYING:
                    if (key == InputHandler::KEY_UP)    game.queueDirection(UP);
                    if (key == InputHandler::KEY_DOWN)  game.queueDirection(DOWN);
                    if (key == InputHandler::KEY_LEFT)  game.queueDirection(LEFT);
                    if (key == InputHandler::KEY_RIGHT) game.queueDirection(RIGHT);
                    if (key == 'p' || key == 'P')  game.togglePause();
                    if (key == 'l' || key == 'L') {
                        game.state = LEADERBOARD;
                        renderer.drawLeaderboard(game);
                    }
                    if (key == 'r' || key == 'R') {
                        // Restart: ask for name again
                        game.nameBuffer = "";
                        game.state = NAME_INPUT;
                        renderer.drawNameInput(game.nameBuffer);
                    }
                    if (key == 'q' || key == 'Q') running = false;
                    break;

                // ---- PAUSED ----
                case PAUSED:
                    if (key == 'p' || key == 'P')  game.togglePause();
                    if (key == 'r' || key == 'R') {
                        game.nameBuffer = "";
                        game.state = NAME_INPUT;
                        renderer.drawNameInput(game.nameBuffer);
                    }
                    if (key == 'q' || key == 'Q') running = false;
                    break;

                // ---- GAME OVER ----
                case GAME_OVER:
                    if (key == 'r' || key == 'R') {
                        // New game: ask for name again
                        game.nameBuffer = "";
                        game.state = NAME_INPUT;
                        renderer.drawNameInput(game.nameBuffer);
                    }
                    else if (key == 'l' || key == 'L') {
                        game.state = LEADERBOARD;
                        renderer.drawLeaderboard(game);
                    }
                    else if (key == 'q' || key == 'Q') {
                        running = false;
                    }
                    break;

                // ---- LEADERBOARD ----
                case LEADERBOARD:
                    if (key == '\n' || key == '\r') {
                        game.nameBuffer = "";
                        game.state = NAME_INPUT;
                        renderer.drawNameInput(game.nameBuffer);
                    }
                    else if (key == 'q' || key == 'Q') {
                        game.state = MENU;
                        renderer.drawMenu();
                    }
                    break;
            }
        }

        // Game tick
        auto now = Clock::now();
        long long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTick).count();

        if (game.state == PLAYING && elapsed >= game.tickInterval) {
            lastTick = now;
            bool alive = game.update();
            if (!alive) renderer.drawGameOver(game);
            else        renderer.drawGame(game);
        }
        else if (game.state == PLAYING || game.state == PAUSED) {
            renderer.drawGame(game);
        }

        input.sleepMs(16);
    }

    renderer.clearScreen();
    std::cout << "\033[?25h\n\n  Thanks for playing! 🐍\n\n";
    return 0;
}
