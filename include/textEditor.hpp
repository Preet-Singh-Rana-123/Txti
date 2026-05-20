#pragma once
#include <fstream>
#include <string>
#include <vector>

#ifndef CTRL
#define CTRL(c) ((c) & 0x1f)
#endif // !DEBUG

struct Cursor {
    int x,y;
};

class TextEditor{
    std::string fileName;
    std::fstream file;
    std::vector<std::string> data;
    Cursor c;
    bool isOpen;

public:
    TextEditor(std::string fileName);
    void openScreen();
    void handleInput(int ch);
};
