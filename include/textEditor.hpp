#pragma once
#include <fstream>
#include <string>
#include <vector>

class TextEditor{
    std::string fileName;
    std::fstream file;
    std::vector<std::string> data;

public:
    TextEditor(std::string fileName);
    void openScreen();
};
