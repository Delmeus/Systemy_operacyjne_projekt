#include <iostream>
#include <thread>
#include <ncurses.h>
#include <vector>
#include <Client.h>
#include <Timer.h>
#include <chrono>
#include <random>
#include <mutex>
#include <condition_variable>
#include <algorithm>
using namespace std;

int DISTRIBUTOR_Y = 10;
int DISTRIBUTOR_X = 40;

int STATIONS_X = 70;
int TOP_STATION_Y = 5;
int MID_STATION_Y = 10;
int BOT_STATION_Y = 15;

int direction = 0; // 0 - up, 1 - right, 2 - down

int MAX_SPEED = 7;

mutex clientMutex;
vector<Client*> clients;

vector<bool> occupancy(3, false);
condition_variable condition;
bool distributorTaken = false;

void director(int& direction, volatile bool& shouldClose){
    while (!shouldClose){
        direction = (direction + 1) % 3;
        this_thread::sleep_for(chrono::seconds(4));
    }
}

void printAll(){
        erase();
        mvprintw(0, 0, "%s", "Antoni Toczynski");
        int color;
        /*
        Printing legend
        */
        mvprintw(15, 0, "%s", "Speed:");
        for(int i = 1; i < 8; i++){
            mvprintw(15 + i, 0, "%s", "-");
            color = 2 + i;
            attron(COLOR_PAIR(color));
            mvprintw(15 + i, 2, "%d", i);
            attroff(COLOR_PAIR(color));
        }
        /*
        Printing clients
        */
        for (auto it = clients.begin(); it != clients.end(); ++it){
                color = 2 + (*it)->getSpeed();
                attron(COLOR_PAIR(color));
                mvprintw((*it)->position.second, (*it)->position.first, "%s", (*it)->name.c_str());
                attroff(COLOR_PAIR(color));
        }
        /*
            Printing corridors
        */
        attron(COLOR_PAIR(10));
        for (int i = 0; i < DISTRIBUTOR_X; i++){
            mvprintw(DISTRIBUTOR_Y - 1, i, "%s", "-");
            mvprintw(DISTRIBUTOR_Y + 1, i, "%s", "-");
        }

        for (int i = TOP_STATION_Y - 1; i <= BOT_STATION_Y + 1; i++){
            if(!(i >= DISTRIBUTOR_Y - 1 && i <= DISTRIBUTOR_Y + 1)){
                mvprintw(i, DISTRIBUTOR_X - 1, "%s", "|");
            }
        }

        for (int i = TOP_STATION_Y + 1; i <= BOT_STATION_Y - 1; i++){
            if(!(i >= DISTRIBUTOR_Y - 1 && i <= DISTRIBUTOR_Y + 1)){
                mvprintw(i, DISTRIBUTOR_X + 1, "%s", "|");
            }
        }

        for (int i = DISTRIBUTOR_X + 1; i <= STATIONS_X; i++){
            mvprintw(TOP_STATION_Y - 1, i, "%s", "-");
            mvprintw(TOP_STATION_Y + 1, i, "%s", "-");
            mvprintw(MID_STATION_Y - 1, i, "%s", "-");
            mvprintw(MID_STATION_Y + 1, i, "%s", "-");
            mvprintw(BOT_STATION_Y - 1, i, "%s", "-");
            mvprintw(BOT_STATION_Y + 1, i, "%s", "-");
        }

        mvprintw(TOP_STATION_Y - 1, DISTRIBUTOR_X, "%s", "-");
        mvprintw(BOT_STATION_Y + 1, DISTRIBUTOR_X, "%s", "-");
        attroff(COLOR_PAIR(10));
        /*
        Printing stations
        */
        if(occupancy[0]) color = 2;
        else color = 1;
        attron(COLOR_PAIR(color));
        mvaddch(TOP_STATION_Y - 1, STATIONS_X, ACS_DARROW);
        mvaddch(TOP_STATION_Y + 1, STATIONS_X, ACS_UARROW);
        attroff(COLOR_PAIR(color));

        if(occupancy[1]) color = 2;
        else color = 1;
        attron(COLOR_PAIR(color));
        mvaddch(MID_STATION_Y - 1, STATIONS_X, ACS_DARROW);
        mvaddch(MID_STATION_Y + 1, STATIONS_X, ACS_UARROW);
        attroff(COLOR_PAIR(color));

        if(occupancy[2]) color = 2;
        else color = 1;
        attron(COLOR_PAIR(color));
        mvaddch(BOT_STATION_Y - 1, STATIONS_X, ACS_DARROW);
        mvaddch(BOT_STATION_Y + 1, STATIONS_X, ACS_UARROW);
        attroff(COLOR_PAIR(color));
        /*
        Printing distributor
        */
        if(distributorTaken){
            color = 2;
        }
        else{
            color = 1;
        }
        attron(COLOR_PAIR(color));
        if (direction == 0){
            mvaddch(DISTRIBUTOR_Y, DISTRIBUTOR_X, ACS_UARROW);
        }
        else if (direction == 1){
            mvaddch(DISTRIBUTOR_Y, DISTRIBUTOR_X, ACS_RARROW);
        }   
        else{
            mvaddch(DISTRIBUTOR_Y, DISTRIBUTOR_X, ACS_DARROW);
        }
        attroff(COLOR_PAIR(color));
        refresh();
}

void janitorThread(volatile bool& shouldClose){
    while(!shouldClose){
        vector<Client*> toDelete;
        
        unique_lock<mutex> lock(clientMutex);

        condition.wait(lock, [&]() {
            for (auto it = clients.begin(); it != clients.end(); ++it) {
                if ((*it)->completed()) {
                    toDelete.push_back(*it);
                }
            }
            return !toDelete.empty() || shouldClose;
        });
        lock.unlock();

        if(shouldClose) break;

        for (auto client : toDelete) {
            client->close(condition);
            delete client;
        }

        lock.lock();
        clients.erase(remove_if(clients.begin(), clients.end(), [&](Client* client) {
            return find(toDelete.begin(), toDelete.end(), client) != toDelete.end();
        }), clients.end());
    }
}

void managerThread(volatile bool& shouldClose){
    srand(time(nullptr));
    int delay = 0;

    int* cords = new int[5]{DISTRIBUTOR_X, DISTRIBUTOR_Y, STATIONS_X, TOP_STATION_Y, BOT_STATION_Y};

    while(!shouldClose){
        delay = rand() % 2 + 1; 
        char name = static_cast<char>(rand() % 25 + 65);
        int speed = rand() % MAX_SPEED + 1;
        string s(1, name);
        
        Client* newClient = new Client(s, speed, ref(direction), cords, clients, clientMutex, occupancy, condition);

        unique_lock<mutex> lock(clientMutex);
        clients.push_back(newClient);
        lock.unlock();
        this_thread::sleep_for(chrono::seconds(delay));

    }

    delete[] cords;
}

int main(int argc, char** argv) {
    
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    resize_term(100, 100);

    start_color();
    /*
    Distributor and stations colors
    */
    init_pair(1, COLOR_BLACK, COLOR_GREEN); 
    init_pair(2, COLOR_BLACK, COLOR_RED);
    /*
    Client colors
    */
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);
    init_pair(5, COLOR_GREEN, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, COLOR_RED, COLOR_BLACK);
    init_pair(8, COLOR_YELLOW, COLOR_BLACK);
    init_pair(9, COLOR_WHITE, COLOR_BLACK);
    /*
    Corridor color
    */
    init_pair(10, 8, 8);

    volatile bool shouldClose = false;
    thread dir_th(director, ref(direction), ref(shouldClose));
    thread manager(managerThread, ref(shouldClose));
    thread janitor(janitorThread, ref(shouldClose));

    while (!shouldClose){
        printAll();
        int ch = getch();
        if (ch == ' ') {
            condition.notify_all();
            shouldClose = true;
        }
        this_thread::sleep_for(chrono::milliseconds(10));
    }
    endwin();
    
    cout << "Stopping threads" << endl;
    dir_th.join();
    cout << "Distributor thread finished" << endl;
    manager.join();
    cout << "Manager thread finished" << endl;
    janitor.join();
    cout << "Janitor thread finished" << endl;

    for (auto it = clients.begin(); it != clients.end(); ++it){
        (*it)->close(condition);
        delete *it;
    }

    cout << "Threads stopped successfully" << endl;

    return 0;
}

