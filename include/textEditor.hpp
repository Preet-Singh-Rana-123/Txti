#pragma once
#include <chrono>
#include <fstream>
#include <stack>
#include <string>
#include <vector>
#include <ncurses.h>

#ifndef CTRL
#define CTRL(c) ((c) & 0x1f)
#endif // !DEBUG

struct Cursor {
    int x,y;
};

enum class ActionType{
    INSERT_TEXT,
    DELETE_TEXT,
};

struct UndoAction{
    ActionType type;
    std::string text;
    int x,y;
};

class TextEditor{
    std::string fileName;
    std::string displayName;
    std::fstream file;
    std::vector<std::string> data;
    Cursor c;
    bool isOpen;
    WINDOW* headerWin;
    WINDOW* textWin;
    WINDOW* footerWin;
    WINDOW* lineCountWin;
    int line_num;
    
    // undo and redo
    std::stack<UndoAction> undoStack;

    std::string batchText;
    int batchX;
    int batchY;

    std::chrono::steady_clock::time_point lastKeyPressTime;
    const std::chrono::milliseconds batchTimeout{3000};

    void pushTypedBatch();
    void recordStructuralAction(ActionType type, const std::string& text, int targetX, int targetY);

public:
    TextEditor(std::string fileName);
    void openScreen();
    void handleInput(int ch);
    void saveData();
    void deleteChar();
    void handleFooter();
    void deleteLine();
    void deleteWord();
    void undoChanges();

    // helper function

    void print_in_middle(WINDOW *win);
};
