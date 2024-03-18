#include <iostream>
#include <thread>
#include <ncurses.h>
#include <vector>
#include <Client.h>

using namespace std;


int main(int argc, char** argv) {
    auto c1 = Client("a", 4, 12);
    auto c2 = Client("b", 10, 20);
    initscr();
    mvprintw(c1.position.first, c1.position.second, "%s", c1.name.c_str());
    mvprintw(c2.position.first, c2.position.second, "%s", c2.name.c_str());
    refresh();
    getch();
    endwin();
    return 0;
}