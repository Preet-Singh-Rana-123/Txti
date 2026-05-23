#include "../include/textEditor.hpp"
#include <iostream>
using namespace std;

int main(int argc, char* argv[]){
    TextEditor t(argv[1]);
    t.openScreen();
    return 0;
}
