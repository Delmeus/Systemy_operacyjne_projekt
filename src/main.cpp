#include <iostream>
#include <thread>
#include <ncurses.h>
#include <vector>
#include <Client.h>
#include <Timer.h>
#include <chrono>
#include <atomic>
#include <random>
#include <map>

using namespace std;

int DIRECTOR_Y = 10;
int DIRECTOR_X = 40;
int ID = 0;

vector<thread> clientThreads;
map<int, Client> clients;

void director(int& direction, bool& shouldClose){
    while (!shouldClose){
        direction = (direction + 1) % 3;
        this_thread::sleep_for(chrono::seconds(4));
    }
    
}

void screenRefresher(bool& shouldClose, int& direction){
    while (!shouldClose){
        clear();
        for (auto it = clients.begin(); it != clients.end(); ++it){
                mvprintw(it->second.position.second, it->second.position.first, "%s", it->second.name.c_str());
                //mvprintw(0,0, "%s", "siema");
        }
       
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
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

void printAll(int direction){
        clear();
        for (auto it = clients.begin(); it != clients.end(); ++it){
                mvprintw(it->second.position.second, it->second.position.first, "%s", it->second.name.c_str());
                //mvprintw(0,0, "%s", "siema");
        }
       
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


void clientThread(Client& client){
    while (true){
        if (client.position.first >= DIRECTOR_X && client.direction == -1){
            break;
        }
        else{
            //mvprintw(client.position.second, client.position.first, "%s", client.name.c_str());
            client.position.first += client.speed;
        }
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

int MAX_ID = 100;

Client createClient(){
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(65, 90);
    
    int randomNumber = dis(gen);

    char name = static_cast<char>(randomNumber);
    srand(time(nullptr));
    int speed = rand() % 3 + 1;
    string s(1, name);
    ID++;
    if(ID > MAX_ID)
        ID = 0;
    return Client(s, speed, ID);
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
    //thread ref_th(screenRefresher, ref(shouldClose), ref(direction));

    srand(time(nullptr));
    int delay = 0;
    Timer timer;
    timer.start();

    bool running = true;
    while (running){
        //clear();
        timer.stop();
        if(timer.mili() > delay * 1000){
            delay = rand() % 10 + 3;
            timer.start();
            Client newClient = createClient();
            clients.insert(std::make_pair(ID, newClient));
            clientThreads.emplace_back(clientThread, ref(newClient));
        }

        printAll(direction);

        int ch = getch();
        if (ch == ' ') {
            running = false;
            shouldClose = true;
        }
    }
    //ref_th.join();
    endwin();
    cout << "Stopping threads" << endl;
    dir_th.join();

    cout << "Number of client threads = " << clientThreads.size() << endl;
    for(auto& th : clientThreads) {
        if (th.joinable()) {
            cout << "Stopping client thread" << endl;
            th.join();
        }
    }

    cout << "Threads stopped successfully" << endl;

    return 0;
}

