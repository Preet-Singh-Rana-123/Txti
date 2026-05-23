#include "../include/textEditor.hpp"
#include <filesystem>
#include <fstream>
#include <string>

TextEditor::TextEditor(std::string fileName){
    this->fileName = std::filesystem::path(fileName).filename().string();
    this->file.open(fileName, std::ios::in | std::ios::out);
    this->isOpen = true;
    this->textWin = nullptr;
    this->headerWin = nullptr;

    if(!file.is_open()){
        std::ofstream createNewFile(fileName);
        createNewFile.close();

        file.open(fileName, std::ios::in | std::ios::out);
    }

    std::string line;
    while(std::getline(file,line)){
        this->data.push_back(line);
    }

    if(this->data.empty()){
        c.x = 0;
        c.y = 0;

        this->data.push_back("");
    }else{
        c.x = data[data.size()-1].size();
        c.y = data.size() - 1;
    }

    this->file.clear();
}

void TextEditor::openScreen(){
    int ch;

    initscr();
    raw();
    keypad(stdscr, true);
    noecho();

    this->headerWin = newwin(1,COLS,0,0);
    this->textWin = newwin(LINES-1,COLS,1,0);

    keypad(this->textWin, true);

    mvwprintw(this->headerWin,0,0,"%s\n",this->fileName.c_str());
    wrefresh(this->headerWin);

    for(std::string line : this->data){
        wprintw(this->textWin,"%s\n",line.c_str());
    }

    wmove(this->textWin,this->c.y,this->c.x);
    wrefresh(this->textWin);

    while(this->isOpen){
        ch = wgetch(this->textWin);
        this->handleInput(ch);
    }

    delwin(this->headerWin);
    delwin(this->textWin);
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
            if(this->c.y == 0){
                this->c.y = 0;
                wmove(this->textWin,this->c.y,this->c.x);
                break;
            }
            this->c.y--;
            if(this->data[this->c.y].size() < this->c.x){
                this->c.x = this->data[this->c.y].size();
            }
            wmove(this->textWin,this->c.y,this->c.x);
            wrefresh(this->textWin);
            break;
        }
        case KEY_DOWN:{
            if(this->c.y == this->data.size() - 1){
                this->c.y = this->data.size()-1;
                wmove(this->textWin,this->c.y,this->c.x);
                break;
            }
            this->c.y++;
            if(this->data[this->c.y].size() < this->c.x){
                this->c.x = this->data[this->c.y].size();
            }
            wmove(this->textWin,this->c.y, this->c.x);
            wrefresh(this->textWin);
            break;
        }
        case KEY_LEFT:{
            if(this->c.x == 0){
                this->c.y--;
                this->c.x = this->data[this->c.y].size();
                wmove(this->textWin,this->c.y,this->c.x);
                wrefresh(this->textWin);
                break;
            }
            this->c.x--;
            wmove(this->textWin,this->c.y,this->c.x);
            wrefresh(this->textWin);
            break;
        }
        case KEY_RIGHT:{
            if(this->c.x == this->data[this->c.y].size()){
                this->c.y++;
                this->c.x = 0;
                wmove(this->textWin,this->c.y,this->c.x);
                wrefresh(this->textWin);
                break;
            }
            this->c.x++;
            wmove(this->textWin,this->c.y,this->c.x);
            wrefresh(this->textWin);
            break;
        }
        case '\n':{
            std::string nextLine = this->data[this->c.y].substr(this->c.x);
            this->data[this->c.y].erase(this->c.x);
            wclrtoeol(this->textWin);
            this->c.y++;
            this->c.x = 0;
            this->data.insert(this->data.begin() + this->c.y,std::move(nextLine));
            wmove(this->textWin,this->c.y, 0);
            wclrtobot(this->textWin);
            for(int i=this->c.y; i < this->data.size(); i++){
                wprintw(this->textWin,"%s\n", this->data[i].c_str());
            }
            wmove(this->textWin,this->c.y,this->c.x);
            wrefresh(this->textWin);
            break;
        }
        default:{
            clrtoeol();
            wprintw(this->textWin,"%c",ch);
            this->data[this->c.y].insert(this->c.x,1,ch);
            this->c.x++;
            wprintw(this->textWin,"%s", data[this->c.y].substr(this->c.x).c_str());
            wmove(this->textWin,this->c.y,this->c.x);
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

        wmove(this->textWin,this->c.y,this->c.x);
        wclrtoeol(this->textWin);
        wprintw(this->textWin,"%s",data[this->c.y].substr(this->c.x).c_str());
        wmove(this->textWin,this->c.y,this->c.x);
    }else if(this->c.y > 0){
        std::string currentLine = this->data[this->c.y];
        this->data.erase(this->data.begin() + this->c.y);
        this->c.y--;
        this->c.x = data[this->c.y].size();
        this->data[this->c.y] += currentLine;

        wmove(this->textWin,this->c.y, 0);
        wclrtobot(this->textWin);
        for(int i=this->c.y; i < this->data.size(); i++){
            wprintw(this->textWin,"%s\n", this->data[i].c_str());
        }
        wmove(this->textWin,this->c.y,this->c.x);
    }
}

