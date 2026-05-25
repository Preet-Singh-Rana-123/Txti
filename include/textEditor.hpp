#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <ncurses.h>

#ifndef CTRL
#define CTRL(c) ((c) & 0x1f)
#endif // !DEBUG

struct Cursor {
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

public:
    TextEditor(std::string fileName);
    void openScreen();
    void handleInput(int ch);
    void saveData();
    void deleteChar();
    void handleFooter();
    void deleteLine();
    void deleteWord();

    // helper function

    void print_in_middle(WINDOW *win);
};
