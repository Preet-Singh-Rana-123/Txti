#include "../include/textEditor.hpp"
#include <fstream>
#include <ncurses.h>
#include <string>

TextEditor::TextEditor(std::string fileName){
    this->fileName = fileName;
    this->file.open(fileName, std::ios::in | std::ios::out);
    c.x = 0;
    c.y = 0;
    this->isOpen = true;

    if(!file.is_open()){
        std::ofstream createNewFile(fileName);
        createNewFile.close();

        file.open(fileName, std::ios::in | std::ios::out);
    }

    std::string line;
    while(std::getline(file,line)){
        this->data.push_back(line);
    }

    c.x = data[data.size()-1].size();
    c.y = data.size() - 1;

    file.clear();
}

void TextEditor::openScreen(){
    int ch;

    initscr();
    raw();
    keypad(stdscr, true);
    noecho();

    for(std::string line : this->data){
        printw("%s\n",line.c_str());
    }

    move(this->c.y,this->c.x);
    refresh();

    while(this->isOpen){
        ch = getch();
        this->handleInput(ch);
    }

    endwin();
}

void TextEditor::handleInput(int ch){
    switch (ch) {
        case CTRL('q'):{
            this->isOpen = false;
        }
        case KEY_BACKSPACE:{
        }
        default:{
            printw("%c",ch);
        }
    }
}
