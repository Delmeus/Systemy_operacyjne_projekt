#include <iostream>
#include <thread>
#include <ncurses.h>
#include <vector>
#include <Client.h>
#include <Timer.h>
#include <chrono>
#include <random>
#include <list>

using namespace std;

int DIRECTOR_Y = 10;
int DIRECTOR_X = 40;


int STATIONS_X = 70;
int TOP_STATION_Y = 5;
int MID_STATION_Y = 10;
int BOT_STATION_Y = 15;

int direction = 0; // 0 - up, 1 - right, 2 - down
//int clientAmount = 0;

vector<thread> clientThreads;
vector<Client> clients;

void director(int& direction, bool& shouldClose){
    while (!shouldClose){
        direction = (direction + 1) % 3;
        this_thread::sleep_for(chrono::seconds(4));
    }
    
}

void printAll(){
        clear();
        mvprintw(0, 0, "%s", "Antoni Toczynski");
        for (auto it = clients.begin(); it != clients.end(); ++it){
                mvprintw(it->position.second, it->position.first, "%s", it->name.c_str());
        }
        /*
            Printing corridors
        */
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


void clientThread(Client client, int index, bool& shouldClose){
    int speed = 1;
    //int index = client.getIndex(clients);
    while (!shouldClose){
        if(client.position.first + speed >= STATIONS_X){
            client.position.first = STATIONS_X;
            clients.at(index).position = client.position;
            this_thread::sleep_for(chrono::seconds(3));
            client.position.first = STATIONS_X + 1;
            clients.at(index).position = client.position;
            this_thread::sleep_for(chrono::seconds(1));
            //clients.erase(next(clients.begin(), index));
            break;
        }      
        /*
         Client sent up
        */  
        else if(client.direction == 0){
            if(client.position.second > TOP_STATION_Y){
                if(client.position.second - speed <= TOP_STATION_Y){
                    client.position.second = TOP_STATION_Y;
                }
                else{
                    client.position.second -= speed;
                }
            }
            else{
                client.position.first += speed;
            }
        }
        /*
         Client sent down
        */         
        else if(client.direction == 2){
            if(client.position.second < BOT_STATION_Y){
                if(client.position.second + speed >= BOT_STATION_Y){
                    client.position.second = BOT_STATION_Y;
                }
                else{
                    client.position.second += speed;
                }
            }
            else{
                client.position.first += speed;
            }
        }
        else if (client.position.first + speed >= DIRECTOR_X && client.direction == -1){
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
        else if((client.position.first + speed < DIRECTOR_X && client.direction == -1) || client.direction == 1){
            client.position.first += speed;
        }

        clients.at(index).position = client.position;
        clients.at(index).direction = client.direction;
        this_thread::sleep_for(chrono::milliseconds(200 + 300 % client.speed));
    }
    //clientAmount = 0;
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
    return Client(s, speed);
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

    //srand(time(nullptr));
    int delay = 0;
    int i = 0;
    Timer timer;
    timer.start();
    int test = 0;
    volatile bool running = true;
    while (running){
        timer.stop();
        if(timer.mili() > delay * 1000){ // && clientAmount == 0
            //clientAmount = 1;
            delay = rand() % 5 + 3;
            timer.start();
            Client newClient = createClient();
            clients.push_back(newClient);
            clientThreads.emplace_back(clientThread, newClient, i, ref(shouldClose));
            i++;
        }
        printAll();

        int ch = getch();
        if (ch == ' ') {
            running = false;
            shouldClose = true;
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }
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
        cout << "client pos = " << it->position.first << " " << it->position.second << endl;
        cout << "client dir = " << it->direction << " client speed = " << it->speed << endl;
    }

    cout << "Threads stopped successfully" << endl;

    return 0;
}

