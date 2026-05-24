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
    this->footerWin = nullptr;
    this->lineCountWin = nullptr;
    this->line_num = 1;

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
    refresh();

    this->headerWin = newwin(3,COLS,0,0);
    this->lineCountWin = newwin(LINES-7,4,3,0);
    this->textWin = newwin(LINES-7,COLS-4,3,4);
    this->footerWin = newwin(4,COLS,LINES-4,0);

    keypad(this->textWin, true);

    box(this->headerWin,0,0);
    this->print_in_middle(headerWin);

    box(this->footerWin,0,0);
    mvwprintw(this->footerWin, 1,2, "Footer Window");
    wrefresh(this->footerWin);

    for(const std::string &line : this->data){
        mvwprintw(this->lineCountWin, this->line_num-1,0,"%2d",line_num);

        wprintw(this->textWin,"%s\n",line.c_str());
        this->line_num++;
    }

    wrefresh(this->lineCountWin);

    wmove(this->textWin,this->c.y,this->c.x);
    wrefresh(this->textWin);

    while(this->isOpen){
        ch = wgetch(this->textWin);
        this->handleInput(ch);
    }


    delwin(this->headerWin);
    delwin(this->textWin);
    delwin(this->footerWin);
    delwin(this->lineCountWin);
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
            mvwprintw(this->lineCountWin,line_num-1,0,"%2d",line_num);
            wrefresh(this->lineCountWin);
            this->line_num++;
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


void TextEditor::print_in_middle(WINDOW *win){
    if (win == nullptr) return;

    int winWidth = getmaxx(win);
    int length = this->fileName.size();
    int startX = (winWidth - length)/2;

    if(startX<0) startX = 0;

    wattron(win, A_BOLD);
    mvwprintw(win,1,startX,"-- %s --",this->fileName.c_str());
    wattroff(win, A_BOLD);
    wrefresh(win);
}
