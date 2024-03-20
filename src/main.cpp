#include <iostream>
#include <thread>
#include <ncurses.h>
#include <vector>
#include <Client.h>
#include <chrono>
#include <atomic>

using namespace std;

int DIRECTOR_Y = 10;
int DIRECTOR_X = 40;

void director(int& direction, bool& shouldClose){
    while (!shouldClose){
        direction = (direction + 1) % 3;
        this_thread::sleep_for(chrono::seconds(1));
    }
    
}

void screenRefresher(bool& shouldClose){
    while (!shouldClose){
        refresh();
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

int main(int argc, char** argv) {
    
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    resize_term(100, 100);
    int direction = 0;
    bool shouldClose = false;
    thread dir_th(director, ref(direction), ref(shouldClose));
    thread ref_th(screenRefresher, ref(shouldClose));
    auto c1 = Client("a", 4, 12, 1);
    auto c2 = Client("b", 10, 20, 2);

    bool running = true;
    while (running){
        clear();
        printw("Current direction %d\n", direction);
        
        if (direction == 0){
            mvaddch(DIRECTOR_Y, DIRECTOR_X, ACS_UARROW);
        }
        else if (direction == 1){
            mvaddch(DIRECTOR_Y, DIRECTOR_X, ACS_RARROW);
        }
        else{
            mvaddch(DIRECTOR_Y, DIRECTOR_X, ACS_DARROW);
        }
        
        // mvprintw(c1.position.first, c1.position.second, "%s", c1.name.c_str());
        // mvprintw(c2.position.first, c2.position.second, "%s", c2.name.c_str());
        // refresh();
        int ch = getch();
        if (ch == ' ') {
            running = false;
            shouldClose = true;
        }
    }
    ref_th.join();
    endwin();
    cout << "Stopping threads" << endl;
    dir_th.join();
    cout << "Threads stopped successfully" << endl;
    return 0;
}

