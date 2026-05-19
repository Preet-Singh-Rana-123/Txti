#include "../include/textEditor.hpp"
#include <ncurses.h>

TextEditor::TextEditor(){
}

void TextEditor::openScreen(){
    initscr();
    printw("Hello World from NCURSES window!");
    refresh();
    getch();
    endwin();
}
