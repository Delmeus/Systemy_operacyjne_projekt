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

class Client {
private:
    // 0 - distributor x
    // 1 - distributor y
    // 2 - all stations x
    // 3 - top station y
    // 4 - bot station y
    int stationCoordinates[5];
    volatile bool shouldClose;
    int speed;
    int direction = -1;
    thread clientThread;

    bool canMove(pair<int, int> nextPosition, const vector<Client*>& clients, const vector<bool>& stationsOccupied){
        if(direction == -1){
            return true;
        }

        for(auto it = clients.begin(); it != clients.end(); ++it){
            if((*it) == this){
                continue;
            }
            if((*it)->position == nextPosition){
                if(nextPosition.first == stationCoordinates[2] - 1 && stationsOccupied[direction] == true){
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

    Client(string n, int speed, int& distributorDirection, const int coordinates[5], const vector<Client*>& clients, mutex& mutex, vector<bool>& stationsOccupied, condition_variable& condition);
    
    string name;
    pair<int, int> position;

    void move(int& distributorDirection, const vector<Client*>& clients, mutex& mutex, vector<bool>& stationsOccupied, condition_variable& condition);
    void close(condition_variable& condition);

    int getSpeed(){
        return speed;
    }

};

#endif // CLIENT_H
