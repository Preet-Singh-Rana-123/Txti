#include "../include/textEditor.hpp"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

TextEditor::TextEditor(std::string fileName) {
  this->fileName = fileName;
  this->displayName = std::filesystem::path(fileName).filename().string();
  this->file.open(fileName, std::ios::in | std::ios::out);
  this->isOpen = true;
  this->textWin = nullptr;
  this->headerWin = nullptr;
  this->footerWin = nullptr;
  this->lineCountWin = nullptr;
  this->line_num = 1;

  if (!file.is_open()) {
    std::ofstream createNewFile(fileName);
    createNewFile.close();

    file.open(fileName, std::ios::in | std::ios::out);
  }

  std::string line;
  while (std::getline(file, line)) {
    this->data.push_back(line);
  }

  if (this->data.empty()) {
    c.x = 0;
    c.y = 0;

    this->data.push_back("");
  } else {
    c.x = data[data.size() - 1].size();
    c.y = data.size() - 1;
  }

  this->file.clear();
}

void TextEditor::openScreen() {
  int ch;

  initscr();
  raw();
  keypad(stdscr, true);
  noecho();
  refresh();

  this->headerWin = newwin(3, COLS, 0, 0);
  this->lineCountWin = newwin(LINES - 7, 4, 3, 0);
  this->textWin = newwin(LINES - 7, COLS - 4, 3, 4);
  this->footerWin = newwin(4, COLS, LINES - 4, 0);

  keypad(this->textWin, true);

  box(this->headerWin, 0, 0);
  this->print_in_middle(headerWin);

  box(this->footerWin, 0, 0);
  handleFooter();
  wrefresh(this->footerWin);

  for (const std::string &line : this->data) {
    mvwprintw(this->lineCountWin, this->line_num - 1, 0, "%2d", line_num);

    wprintw(this->textWin, "%s\n", line.c_str());
    this->line_num++;
  }

  wrefresh(this->lineCountWin);

  wmove(this->textWin, this->c.y, this->c.x);
  wrefresh(this->textWin);

  while (this->isOpen) {
    ch = wgetch(this->textWin);
    this->handleInput(ch);
  }

  delwin(this->headerWin);
  delwin(this->textWin);
  delwin(this->footerWin);
  delwin(this->lineCountWin);
  endwin();
}

void TextEditor::handleInput(int ch) {
  switch (ch) {
  case CTRL('x'): {
    this->pushTypedBatch();
    this->isOpen = false;
    break;
  }
  case CTRL('a'): {
    this->pushTypedBatch();
    this->saveData();
    break;
  }
  case CTRL('d'): {
    this->pushTypedBatch();
    this->deleteLine();
    break;
  }
  case KEY_BACKSPACE:
  case 127:
  case 8: {
    this->pushTypedBatch();
    this->deleteChar();
    break;
  }
  case CTRL('w'):
  case CTRL(KEY_BACKSPACE): {
    this->pushTypedBatch();
    this->deleteWord();
    break;
  }
  case CTRL('y'): {
    this->redoChanges();
    break;
  }
  case CTRL('z'): {
    this->undoChanges();
    break;
  }
  case KEY_UP: {
    if (this->c.y == 0) {
      this->c.y = 0;
      wmove(this->textWin, this->c.y, this->c.x);
      break;
    }
    this->c.y--;
    if (this->data[this->c.y].size() < this->c.x) {
      this->c.x = this->data[this->c.y].size();
    }
    wmove(this->textWin, this->c.y, this->c.x);
    wrefresh(this->textWin);
    break;
  }
  case KEY_DOWN: {
    if (this->c.y == this->data.size() - 1) {
      this->c.y = this->data.size() - 1;
      wmove(this->textWin, this->c.y, this->c.x);
      break;
    }
    this->c.y++;
    if (this->data[this->c.y].size() < this->c.x) {
      this->c.x = this->data[this->c.y].size();
    }
    wmove(this->textWin, this->c.y, this->c.x);
    wrefresh(this->textWin);
    break;
  }
  case KEY_LEFT: {
    if (this->c.x == 0) {
      this->c.y--;
      this->c.x = this->data[this->c.y].size();
      wmove(this->textWin, this->c.y, this->c.x);
      wrefresh(this->textWin);
      break;
    }
    this->c.x--;
    wmove(this->textWin, this->c.y, this->c.x);
    wrefresh(this->textWin);
    break;
  }
  case KEY_RIGHT: {
    if (this->c.x == this->data[this->c.y].size()) {
      this->c.y++;
      this->c.x = 0;
      wmove(this->textWin, this->c.y, this->c.x);
      wrefresh(this->textWin);
      break;
    }
    this->c.x++;
    wmove(this->textWin, this->c.y, this->c.x);
    wrefresh(this->textWin);
    break;
  }
  case '\n': {
    std::string textMovedToNextLine = this->data[this->c.y].substr(this->c.x);
    recordStructuralAction(ActionType::SPLIT_LINE, textMovedToNextLine,
                           this->c.x, this->c.y);

    std::string nextLine = this->data[this->c.y].substr(this->c.x);
    this->data[this->c.y].erase(this->c.x);
    wclrtoeol(this->textWin);
    this->c.y++;
    this->c.x = 0;
    this->data.insert(this->data.begin() + this->c.y, std::move(nextLine));
    wmove(this->textWin, this->c.y, 0);
    mvwprintw(this->lineCountWin, line_num - 1, 0, "%2d", line_num);
    wrefresh(this->lineCountWin);
    this->line_num++;
    wclrtobot(this->textWin);
    for (int i = this->c.y; i < this->data.size(); i++) {
      wprintw(this->textWin, "%s\n", this->data[i].c_str());
    }
    wmove(this->textWin, this->c.y, this->c.x);
    wrefresh(this->textWin);
    break;
  }
  default: {
    if (ch >= 32 && ch <= 126) {
      auto currentTime = std::chrono::steady_clock::now();
      auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
          currentTime - this->lastKeyPressTime);

      if (elapsed > batchTimeout) {
        this->pushTypedBatch();
      }

      if (this->batchText.empty()) {
        this->batchX = this->c.x;
        this->batchY = this->c.y;
      }

      this->batchText += static_cast<char>(ch);
      this->lastKeyPressTime = currentTime;

      wclrtoeol(this->textWin);
      wprintw(this->textWin, "%c", ch);
      this->data[this->c.y].insert(this->c.x, 1, ch);
      this->c.x++;
      wprintw(this->textWin, "%s", data[this->c.y].substr(this->c.x).c_str());
      wmove(this->textWin, this->c.y, this->c.x);
    }
    break;
  }
  }
}

void TextEditor::saveData() {
  if (this->file.is_open()) {
    this->file.close();
  }

  this->file.open(this->fileName, std::ios::out | std::ios::trunc);
  if (!this->file.is_open())
    return;

  for (std::string &line : this->data) {
    this->file << line << "\n";
  }

  this->file.flush();
  this->file.close();

  this->file.open(this->fileName, std::ios::in | std::ios::out);
}

void TextEditor::deleteChar() {
  if (this->c.x > 0) {
    std::string textToBeDeleted = this->data[c.y].substr(this->c.x - 1, 1);
    this->recordStructuralAction(ActionType::DELETE_TEXT, textToBeDeleted,
                                 this->c.x - 1, this->c.y);
    this->data[this->c.y].erase(this->c.x - 1, 1);
    this->c.x--;

    wmove(this->textWin, this->c.y, this->c.x);
    wclrtoeol(this->textWin);
    wprintw(this->textWin, "%s", data[this->c.y].substr(this->c.x).c_str());
    wmove(this->textWin, this->c.y, this->c.x);
  } else if (this->c.y > 0) {
    std::string currentLine = this->data[this->c.y];
    this->data.erase(this->data.begin() + this->c.y);
    this->c.y--;
    this->c.x = data[this->c.y].size();
    this->data[this->c.y] += currentLine;
    this->line_num--;
    wmove(this->lineCountWin, this->c.y, 0);
    wclrtobot(this->lineCountWin);
    for (size_t i = this->c.y; i < this->data.size(); i++) {
      mvwprintw(this->lineCountWin, i, 0, "%2d", (int)(i + 1));
    }
    wrefresh(this->lineCountWin);

    wmove(this->textWin, this->c.y, 0);
    wclrtobot(this->textWin);
    for (int i = this->c.y; i < this->data.size(); i++) {
      wprintw(this->textWin, "%s\n", this->data[i].c_str());
    }
    wmove(this->textWin, this->c.y, this->c.x);
  }
}

void TextEditor::deleteLine() {
  if (this->data.empty())
    return;

  std::string textToBeDeleted = this->data[c.y];
  this->recordStructuralAction(ActionType::DELETE_LINE, textToBeDeleted, 0,
                               this->c.y);

  this->data.erase(this->data.begin() + this->c.y);
  this->line_num--;

  if (this->data.empty()) {
    this->c.x = 0;
    this->c.y = 0;

    wclear(this->textWin);
    wclear(this->lineCountWin);
    wrefresh(this->textWin);
    wrefresh(this->lineCountWin);
    return;
  }

  if (this->c.y >= this->data.size()) {
    this->c.y = this->data.size() - 1;
  }
  if (this->c.x > this->data[this->c.y].size()) {
    this->c.x = this->data[this->c.y].size();
  }

  wmove(this->lineCountWin, this->c.y, 0);
  wclrtobot(this->lineCountWin);
  for (size_t i = this->c.y; i < this->data.size(); i++) {
    mvwprintw(this->lineCountWin, i, 0, "%2d", (int)(i + 1));
  }
  wrefresh(this->lineCountWin);

  wmove(this->textWin, this->c.y, 0);
  wclrtobot(this->textWin);
  for (int i = this->c.y; i < this->data.size(); i++) {
    wprintw(this->textWin, "%s\n", this->data[i].c_str());
  }
  wmove(this->textWin, this->c.y, this->c.x);
  wrefresh(this->textWin);
}

void TextEditor::deleteWord() {
  int end = this->c.x;
  int start = this->c.x;

  if (start == 0) {
    deleteChar();
    return;
  }

  while (start > 0 && this->data[this->c.y][start - 1] != ' ') {
    start--;
  }

  if (start == end && end > 0) {
    start--;
  }

  std::string textToBeDeleted = this->data[c.y].substr(start, end - start);
  this->recordStructuralAction(ActionType::DELETE_TEXT, textToBeDeleted, start,
                               this->c.y);

  this->data[this->c.y].erase(this->data[this->c.y].begin() + start,
                              this->data[this->c.y].begin() + end);

  this->c.x = start;
  wmove(this->textWin, this->c.y, this->c.x);
  wclrtoeol(this->textWin);
  wprintw(this->textWin, "%s", data[this->c.y].substr(this->c.x).c_str());
  wmove(this->textWin, this->c.y, this->c.x);
  wrefresh(this->textWin);
}

void TextEditor::undoChanges() {
  this->pushTypedBatch();

  if (this->undoStack.empty()) {
    return;
  }

  UndoAction action = undoStack.top();
  undoStack.pop();

  switch (action.type) {
  case ActionType::INSERT_TEXT: {
    if (action.y < this->data.size()) {
      this->data[action.y].erase(action.x, action.text.length());
      this->c.x = action.x;
      this->c.y = action.y;
    }
    break;
  }
  case ActionType::DELETE_TEXT: {
    if (action.y < this->data.size()) {
      data[action.y].insert(action.x, action.text);
    } else {
      data.push_back(action.text);
    }

    this->c.x = action.x;
    this->c.y = action.y;
    break;
  }
  case ActionType::SPLIT_LINE: {
    if (action.y + 1 < this->data.size()) {
      this->data[action.y] += this->data[action.y + 1];
      this->data.erase(this->data.begin() + action.y + 1);
      this->line_num--;
    }
    this->c.x = action.x;
    this->c.y = action.y;
    break;
  }
  case ActionType::MERGE_LINE: {
    std::string fullLineText = this->data[action.y];
    std::string remainingLine = fullLineText.substr(0, action.x);
    std::string restoredLine = fullLineText.substr(action.x);

    this->data[action.y] = remainingLine;
    this->data.insert(this->data.begin() + action.y + 1, restoredLine);
    this->line_num++;

    this->c.x = 0;
    this->c.y = action.y + 1;
    break;
  }
  case ActionType::DELETE_LINE: {
    if (action.y <= this->data.size()) {
      this->data.insert(this->data.begin() + action.y, action.text);
    } else {
      this->data.push_back(action.text);
    }
    this->line_num++;

    this->c.x = action.x;
    this->c.y = action.y;
    break;
  }
  }

  this->redoStack.push(action);

  wclear(this->lineCountWin);
  wclear(this->textWin);

  this->line_num = 1;
  for (const std::string &line : this->data) {
    mvwprintw(this->lineCountWin, this->line_num - 1, 0, "%2d", line_num);

    wprintw(this->textWin, "%s\n", line.c_str());
    this->line_num++;
  }

  wmove(this->textWin, this->c.y, this->c.x);

  wrefresh(this->lineCountWin);
  wrefresh(this->textWin);
}

void TextEditor::redoChanges() {
  this->pushTypedBatch();

  if (this->redoStack.empty()) {
    return;
  }

  UndoAction action = redoStack.top();
  redoStack.pop();

  switch (action.type) {
  case ActionType::INSERT_TEXT: {
    if (action.y < this->data.size()) {
      this->data[action.y].insert(action.x, action.text);
      this->c.x = action.x + action.text.length();
      this->c.y = action.y;
    }
    break;
  }
  case ActionType::DELETE_TEXT: {
    if (action.y < this->data.size()) {
      data[action.y].erase(action.x, action.text.length());
      this->c.x = action.x;
      this->c.y = action.y;
    }
    break;
  }
  case ActionType::SPLIT_LINE: {
    std::string fullLineText = this->data[action.y];
    std::string remainingLine = fullLineText.substr(0, action.x);
    std::string restoredLine = fullLineText.substr(action.x);

    this->data[action.y] = remainingLine;
    this->data.insert(this->data.begin() + action.y + 1, restoredLine);
    this->line_num++;

    this->c.x = 0;
    this->c.y = action.y + 1;
    break;
  }
  case ActionType::MERGE_LINE: {
    if (action.y + 1 < this->data.size()) {
      this->data[action.y] += this->data[action.y + 1];
      this->data.erase(this->data.begin() + action.y + 1);
      this->line_num--;
    }
    this->c.x = action.x;
    this->c.y = action.y;
    break;
  }
  case ActionType::DELETE_LINE: {
    if (action.y < this->data.size()) {
      this->data.erase(this->data.begin() + action.y);
      this->line_num--;

      if (this->data.empty()) {
        this->c.x = 0;
        this->c.y = 0;
      } else {
        if (this->c.y >= this->data.size())
          this->c.y = this->data.size() - 1;
        if (this->c.x > this->data[this->c.y].size())
          this->c.x = this->data[this->c.y].size();
      }
    }
    break;
  }
  }

  this->undoStack.push(action);

  wclear(this->lineCountWin);
  wclear(this->textWin);

  this->line_num = 1;
  for (const std::string &line : this->data) {
    mvwprintw(this->lineCountWin, this->line_num - 1, 0, "%2d", line_num);

    wprintw(this->textWin, "%s\n", line.c_str());
    this->line_num++;
  }

  wmove(this->textWin, this->c.y, this->c.x);

  wrefresh(this->lineCountWin);
  wrefresh(this->textWin);
}

void TextEditor::pushTypedBatch() {
  if (!this->batchText.empty()) {

    while (!redoStack.empty()) {
      redoStack.pop();
    }

    undoStack.push(
        {ActionType::INSERT_TEXT, this->batchText, this->batchX, this->batchY});
    this->batchText = "";
  }
}

void TextEditor::recordStructuralAction(ActionType type,
                                        const std::string &text, int targetX,
                                        int targetY) {
  this->pushTypedBatch();

  while (!redoStack.empty()) {
    redoStack.pop();
  }

  undoStack.push({type, text, targetX, targetY});
}

void TextEditor::print_in_middle(WINDOW *win) {
  if (win == nullptr)
    return;

  int winWidth = getmaxx(win);
  int length = this->displayName.size();
  int startX = (winWidth - length) / 2;

  if (startX < 0)
    startX = 0;

  wattron(win, A_BOLD);
  mvwprintw(win, 1, startX, "-- %s --", this->displayName.c_str());
  wattroff(win, A_BOLD);
  wrefresh(win);
}

void TextEditor::handleFooter() {
  wattron(this->footerWin, A_BOLD);
  mvwprintw(this->footerWin, 1, 2, "Footer Window:- ");
  mvwprintw(this->footerWin, 1, 18, "| CTRL+x -> exit");
  mvwprintw(this->footerWin, 1, 2 * 18 + 2, "| CTRL+a -> save");
  mvwprintw(this->footerWin, 1, 3 * 18 + 2, "| CTRL+d -> delete line");
  mvwprintw(this->footerWin, 1, 4 * 20 + 2, "| CTRL+w -> delete word");
  mvwprintw(this->footerWin, 2, 18, "| CTRL+z -> undo");
  mvwprintw(this->footerWin, 2, 2 * 18 + 2, "| CTRL+y -> redo");
  wattroff(this->footerWin, A_BOLD);
}
