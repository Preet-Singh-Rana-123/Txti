#include "../include/textEditor.hpp"
#include <filesystem>
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
        case CTRL('x'):{
            this->isOpen = false;
            break;
        }
        case CTRL('a'):{
            this->saveData();
            break;
        }
        case KEY_BACKSPACE:
        case 127:
        case 8:{
            this->deleteChar();
            break;
        }
        case KEY_UP:{
            break;
        }
        case KEY_DOWN:{}
        case KEY_LEFT:{}
        case KEY_RIGHT:{}
        case '\n':{
            data.push_back("");
            this->c.x = 0;
            this->c.y++;
            move(this->c.y,this->c.x);
            refresh();
            break;
        }
        default:{
            printw("%c",ch);
            this->c.x++;
            data[data.size()-1].push_back(ch);
            break;
        }
    }
}

void TextEditor::saveData(){
    if(!this->file.is_open()) return;

    this->file.clear();

    this->file.seekp(0,std::ios::beg);

    for(std::string &line : this->data){
        this->file << line <<"\n";
    }

    std::streampos newSize = this->file.tellp();

    this->file.flush();

    std::filesystem::resize_file(this->fileName, newSize);
}

void TextEditor::deleteChar(){
    if(this->c.x > 0){
        this->data[this->c.y].erase(this->c.x - 1,1);
        this->c.x--;

        move(this->c.y,this->c.x);
        clrtoeol();
        printw("%s",data[this->c.y].substr(this->c.x).c_str());
    }else if(this->c.y > 0){
        std::string currentLine = this->data[this->c.y];
        this->c.y--;
        this->c.x = data[this->c.y].size();
        this->data[this->c.y] += currentLine;

        move(this->c.y,this->c.x);
        refresh();
        clear();
        for(std::string& line : data){
            printw("%s\n",line.c_str());
        }
    }
}
