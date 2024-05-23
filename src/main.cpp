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

int DIRECTOR_Y = 10;
int DIRECTOR_X = 40;

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

void director(int& direction, volatile bool& shouldClose){
    while (!shouldClose){
        direction = (direction + 1) % 3;
        this_thread::sleep_for(chrono::seconds(4));
    }
}

void printAll(){
        erase();
        mvprintw(0, 0, "%s", "Antoni Toczynski");
        for (auto it = clients.begin(); it != clients.end(); ++it){
                mvprintw((*it)->position.second, (*it)->position.first, "%s", (*it)->name.c_str());
        }
        /*
            Printing corridors
        */
        for (int i = 0; i < DIRECTOR_X; i++){
            mvprintw(DIRECTOR_Y - 1, i, "%s", "-");
            mvprintw(DIRECTOR_Y + 1, i, "%s", "-");
        }

        for (int i = TOP_STATION_Y; i <= BOT_STATION_Y; i++){
            if(!(i >= DIRECTOR_Y - 1 && i <= DIRECTOR_Y + 1)){
                mvprintw(i, DIRECTOR_X - 1, "%s", "|");
            }
        }

        for (int i = TOP_STATION_Y + 1; i <= BOT_STATION_Y - 1; i++){
            if(!(i >= DIRECTOR_Y - 1 && i <= DIRECTOR_Y + 1)){
                mvprintw(i, DIRECTOR_X + 1, "%s", "|");
            }
        }

        for (int i = DIRECTOR_X + 1; i <= STATIONS_X; i++){
            mvprintw(TOP_STATION_Y - 1, i, "%s", "-");
            mvprintw(TOP_STATION_Y + 1, i, "%s", "-");
            mvprintw(MID_STATION_Y - 1, i, "%s", "-");
            mvprintw(MID_STATION_Y + 1, i, "%s", "-");
            mvprintw(BOT_STATION_Y - 1, i, "%s", "-");
            mvprintw(BOT_STATION_Y + 1, i, "%s", "-");
        }

        mvprintw(TOP_STATION_Y - 1, DIRECTOR_X, "%s", "-");
        mvprintw(BOT_STATION_Y + 1, DIRECTOR_X, "%s", "-");

        mvaddch(TOP_STATION_Y - 1, STATIONS_X, ACS_DARROW);
        mvaddch(MID_STATION_Y - 1, STATIONS_X, ACS_DARROW);
        mvaddch(BOT_STATION_Y - 1, STATIONS_X, ACS_DARROW);

        mvaddch(TOP_STATION_Y + 1, STATIONS_X, ACS_UARROW);
        mvaddch(MID_STATION_Y + 1, STATIONS_X, ACS_UARROW);
        mvaddch(BOT_STATION_Y + 1, STATIONS_X, ACS_UARROW);
       
        if (direction == 0){
            mvaddch(DIRECTOR_Y, DIRECTOR_X, ACS_UARROW);
        }
        else if (direction == 1){
            mvaddch(DIRECTOR_Y, DIRECTOR_X, ACS_RARROW);
        }   
        else{
            mvaddch(DIRECTOR_Y, DIRECTOR_X, ACS_DARROW);
        }

        refresh();
}

void janitorThread(volatile bool& shouldClose){
    /*while (!shouldClose) {
        unique_lock<mutex> lock(clientMutex);

        condition.wait(lock, [&]() {
            for (auto client : clients) {
                if (client->completed()) {
                    return true;
                }
            }
            return shouldClose;
        });

        if (shouldClose) break;
        lock.unlock();
        auto it = clients.begin();
        while (it != clients.end()) {
            if ((*it)->completed()) {
                (*it)->close(condition);
                delete *it;
                lock.lock();
                it = clients.erase(it);
                lock.unlock();
            } else {
                ++it;
            }
        }
    }*/
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

    int* cords = new int[5]{DIRECTOR_X, DIRECTOR_Y, STATIONS_X, TOP_STATION_Y, BOT_STATION_Y};

    while(!shouldClose){
        delay = rand() % 1 + 2; 
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
        this_thread::sleep_for(chrono::milliseconds(100));
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

    // for (auto it = clients.begin(); it != clients.end(); ++it) {
    //     delete *it; 
    // }

    cout << "Threads stopped successfully" << endl;

    return 0;
}

