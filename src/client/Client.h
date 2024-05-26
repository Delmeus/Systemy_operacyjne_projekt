#ifndef CLIENT_H
#define CLIENT_H

#include <utility>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;

extern bool distributorOccupied;
extern int distributorDirection;
extern condition_variable condition;

extern mutex clientMutex;
extern vector<bool> stationsOccupied;

const extern int DISTRIBUTOR_X;
const extern int DISTRIBUTOR_Y;
const extern int STATIONS_X;
const extern int TOP_STATION_Y;
const extern int BOT_STATION_Y;


class Client {
private:
    volatile bool shouldClose;
    int speed;
    int direction = -1;
    thread clientThread;

    bool canMove(pair<int, int> nextPosition, const vector<Client*>& clients){
        if(direction == -1){
            return true;
        }

        for(auto it = clients.begin(); it != clients.end(); ++it){
            if((*it) == this){
                continue;
            }
            if((*it)->position == nextPosition){
                if(nextPosition.first == STATIONS_X - 1 && stationsOccupied[direction] == true){
                    return true;
                }
                return false;
            }
        }
        
        return true;
    }

public: 

    bool operator==(const Client& other) const {
        return name == other.name && position == other.position; 
    }

    bool completed(){
        if(direction == -2)
            return true;
        return false;
    }

    Client(string n, int speed, const vector<Client*>& clients);
    
    string name;
    pair<int, int> position;

    void move(const vector<Client*>& clients);
    void close();

    int getSpeed(){
        return speed;
    }

};

#endif // CLIENT_H
