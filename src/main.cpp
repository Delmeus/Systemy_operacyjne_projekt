#include <iostream>
#include <thread>
#include <ncurses.h>
#include <vector>
#include <Client.h>
#include <chrono>
#include <atomic>

using namespace std;

void director(int& direction, bool& shouldClose){
    while (!shouldClose){
        direction = (direction + 1) % 3;
        this_thread::sleep_for(chrono::seconds(1));
    }
    
}

void screenRefresher(){
    refresh();
}

int main(int argc, char** argv) {
    
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    int direction = 0;
    bool shouldClose = false;
    thread dir_th(director, ref(direction), ref(shouldClose));
    auto c1 = Client("a", 4, 12);
    auto c2 = Client("b", 10, 20);

    bool running = true;
    while (running){
        clear();
        printw("Current direction %d\n", direction);
        // mvprintw(c1.position.first, c1.position.second, "%s", c1.name.c_str());
        // mvprintw(c2.position.first, c2.position.second, "%s", c2.name.c_str());
        refresh();
        int ch = getch();
        if (ch == ' ') {
            running = false;
            shouldClose = true;
        }
    }
    
    endwin();
    cout << "exit1" << endl;
    dir_th.join();
    cout << "exit2" << endl;
    return 0;
}

