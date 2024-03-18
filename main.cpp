#include <iostream>
#include <thread>
#include <ncurses.h>
#include <vector>

using namespace std;


int main(int argc, char** argv) {
    initscr();
    printw("Siemano");
    refresh();
    getch();
    endwin();
    return 0;
}