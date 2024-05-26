#include <iostream>
#include <thread>
#include <ncurses.h>
#include <vector>
#include <Client.h>
#include <chrono>
#include <random>
#include <mutex>
#include <condition_variable>
#include <algorithm>
using namespace std;

const int DISTRIBUTOR_Y = 10;
const int DISTRIBUTOR_X = 40;

const int STATIONS_X = 70;
const int TOP_STATION_Y = 5;
const int MID_STATION_Y = 10;
const int BOT_STATION_Y = 15;

const int MAX_SPEED = 7;

vector<Client*> clients;

int distributorDirection = 0; // 0 - up, 1 - right, 2 - down
bool distributorOccupied = false;

mutex clientMutex;
condition_variable condition;

vector<bool> stationsOccupied(3, false);

void director(int& distributorDirection, volatile bool& shouldClose){
    while (!shouldClose){
        distributorDirection = (distributorDirection + 1) % 3;
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
        if(stationsOccupied[0]) color = 2;
        else color = 1;
        attron(COLOR_PAIR(color));
        mvaddch(TOP_STATION_Y - 1, STATIONS_X, ACS_DARROW);
        mvaddch(TOP_STATION_Y + 1, STATIONS_X, ACS_UARROW);
        attroff(COLOR_PAIR(color));

        if(stationsOccupied[1]) color = 2;
        else color = 1;
        attron(COLOR_PAIR(color));
        mvaddch(MID_STATION_Y - 1, STATIONS_X, ACS_DARROW);
        mvaddch(MID_STATION_Y + 1, STATIONS_X, ACS_UARROW);
        attroff(COLOR_PAIR(color));

        if(stationsOccupied[2]) color = 2;
        else color = 1;
        attron(COLOR_PAIR(color));
        mvaddch(BOT_STATION_Y - 1, STATIONS_X, ACS_DARROW);
        mvaddch(BOT_STATION_Y + 1, STATIONS_X, ACS_UARROW);
        attroff(COLOR_PAIR(color));
        /*
        Printing distributor
        */
        if(distributorOccupied){
            color = 2;
        }
        else{
            color = 1;
        }
        attron(COLOR_PAIR(color));
        if (distributorDirection == 0){
            mvaddch(DISTRIBUTOR_Y, DISTRIBUTOR_X, ACS_UARROW);
        }
        else if (distributorDirection == 1){
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
            client->close();
            delete client;
        }

        lock.lock();
        clients.erase(remove_if(clients.begin(), clients.end(), [&](Client* client) {
            return find(toDelete.begin(), toDelete.end(), client) != toDelete.end();
        }), clients.end());
    }
}

void generatorThread(volatile bool& shouldClose){
    srand(time(nullptr));
    int delay = 0;

    while(!shouldClose){
        delay = rand() % 2 + 1; 
        char name = static_cast<char>(rand() % 25 + 65);
        int speed = rand() % MAX_SPEED + 1;
        string s(1, name);
        
        Client* newClient = new Client(s, speed, clients);

        unique_lock<mutex> lock(clientMutex);
        clients.push_back(newClient);
        lock.unlock();
        this_thread::sleep_for(chrono::seconds(delay));

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
    thread dir_th(director, ref(distributorDirection), ref(shouldClose));
    thread generator(generatorThread, ref(shouldClose));
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
    generator.join();
    cout << "Generator thread finished" << endl;
    janitor.join();
    cout << "Janitor thread finished" << endl;

    for (auto it = clients.begin(); it != clients.end(); ++it){
        (*it)->close();
        delete *it;
    }

    cout << "Threads stopped successfully" << endl;

    return 0;
}

