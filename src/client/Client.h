#ifndef CLIENT_H
#define CLIENT_H

#include <utility>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;

class Client {
private:
    // 0 - director x
    // 1 - director y
    // 2 - all stations x
    // 3 - top station y
    // 4 - bot station y
    int stationCoordinates[5];
    bool shouldClose;
    int speed;
    int direction = -1;
    thread clientThread;

    bool canMove(pair<int, int> nextPosition, const vector<Client*>& clients){
        if(direction == -1 || nextPosition.first == stationCoordinates[2] - 1){
            return true;
        }

        for(auto it = clients.begin(); it != clients.end(); ++it){
            if((*it) == this){
                continue;
            }
            if((*it)->position == nextPosition){
                //speed = (*it)->speed;
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

    Client(string n, int speed, int& distributorDirection, const int coordinates[5], const vector<Client*>& clients, mutex& mutex);
    
    string name;
    pair<int, int> position;

    int getIndex(const vector<Client*>& clients) const;

    void move(int& distributorDirection, const vector<Client*>& clients, mutex& mutex);
    void close();

};

#endif // CLIENT_H
