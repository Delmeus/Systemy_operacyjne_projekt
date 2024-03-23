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

int STATIONS_X = 70;
int TOP_STATION_Y = 5;
int MID_STATION_Y = 10;
int BOT_STATION_Y = 15;

int direction = 0; // 0 - up, 1 - right, 2 - down
int clientAmount = 0;

vector<thread> clientThreads;
map<int, Client> clients;

void director(int& direction, bool& shouldClose){
    while (!shouldClose){
        direction = (direction + 1) % 3;
        this_thread::sleep_for(chrono::seconds(4));
    }
    
}

void printAll(){
        clear();
        for (auto it = clients.begin(); it != clients.end(); ++it){
                mvprintw(it->second.position.second, it->second.position.first, "%s", it->second.name.c_str());
        }

        for (int i = 0; i < DIRECTOR_X; i++){
            mvprintw(DIRECTOR_Y - 1, i, "%s", "-");
            mvprintw(DIRECTOR_Y + 1, i, "%s", "-");
        }

        for (int i = TOP_STATION_Y - 1; i <= BOT_STATION_Y + 1; i++){
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

// void screenRefresher(bool& shouldClose){
//     while (!shouldClose){
//         printAll();
//         this_thread::sleep_for(chrono::milliseconds(300));
//     }
// }

void clientThread(Client& client, bool& shouldClose){
    while (!shouldClose){
        if(client.position.first + client.speed >= STATIONS_X){
            client.position.first = STATIONS_X;
            clients.at(client.id).position = client.position;
            this_thread::sleep_for(chrono::seconds(3));
            client.position.first = STATIONS_X + 1;
            clients.at(client.id).position = client.position;
            this_thread::sleep_for(chrono::seconds(1));
            clients.erase(client.id);
            break;
        }      
        /*
         Client sent up
        */  
        else if(client.direction == 0){
            if(client.position.second < TOP_STATION_Y){
                if(client.position.second - client.speed <= TOP_STATION_Y){
                    client.position.second = TOP_STATION_Y;
                }
                else{
                    client.position.second -= client.speed;
                }
            }
            else{
                client.position.first += client.speed;
            }
        }
        /*
         Client sent down
        */         
        else if(client.direction == 2){
            if(client.position.second < BOT_STATION_Y){
                if(client.position.second + client.speed >= BOT_STATION_Y){
                    client.position.second = BOT_STATION_Y;
                }
                else{
                    client.position.second += client.speed;
                }
            }
            else{
                client.position.first += client.speed;
            }
        }
        else if (client.position.first + client.speed >= DIRECTOR_X && client.direction == -1){
            if (direction == 0){
                client.position = make_pair(DIRECTOR_X, DIRECTOR_Y - 1);
                client.direction = 0;
            }
            else if (direction == 2){
                client.position = make_pair(DIRECTOR_X, DIRECTOR_Y + 1);
                client.direction = 2;
            }
            else{
                client.position = make_pair(DIRECTOR_X + 1, DIRECTOR_Y);
                client.direction = 1;
            }
        }
        else if((client.position.first + client.speed < DIRECTOR_X && client.direction == -1) || client.direction == 1){
            client.position.first += client.speed;
        }

        clients.at(client.id).position = client.position;
        this_thread::sleep_for(chrono::seconds(1));
    }
    clientAmount = 0;
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
    bool shouldClose = false;
    thread dir_th(director, ref(direction), ref(shouldClose));
    //thread ref_th(screenRefresher, ref(shouldClose));

    srand(time(nullptr));
    int delay = 0;
    Timer timer;
    timer.start();
    int test = 0;
    bool running = true;
    while (running){
        timer.stop();
        if(timer.mili() > delay * 1000 && clientAmount == 0){
            clientAmount = 1;
            delay = rand() % 10 + 3;
            timer.start();
            Client newClient = createClient();
            clients.insert(std::make_pair(ID, newClient));
            clientThreads.emplace_back(clientThread, ref(newClient), ref(shouldClose));
        }

        printAll();

        int ch = getch();
        if (ch == ' ') {
            running = false;
            shouldClose = true;
        }
        this_thread::sleep_for(chrono::milliseconds(100));
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

    for (auto it = clients.begin(); it != clients.end(); ++it){
        cout << "client pos = " << it->second.position.first << " " << it->second.position.second << endl;
        cout << "client dir = " << it->second.direction << " client speed = " << it->second.speed << endl;
        cout << it->first << " " << it->second.id << endl;
    }

    cout << "Threads stopped successfully" << endl;

    return 0;
}

