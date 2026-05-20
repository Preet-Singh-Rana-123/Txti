#include "../include/textEditor.hpp"
#include <fstream>
#include <ncurses.h>
#include <string>

TextEditor::TextEditor(std::string fileName){
    this->fileName = fileName;
    this->file.open(fileName, std::ios::in | std::ios::out);

    if(!file.is_open()){
        std::ofstream createNewFile(fileName);
        createNewFile.close();

        file.open(fileName, std::ios::in | std::ios::out);
    }

    std::string line;
    while(std::getline(file,line)){
        this->data.push_back(line);
    }

    file.clear();
}

void TextEditor::openScreen(){
    initscr();
    for(std::string line : this->data){
        printw("%s\n",line.c_str());
    }
    refresh();
    getch();
    endwin();
}
