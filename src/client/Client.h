#ifndef CLIENT_H
#define CLIENT_H

#include <utility>
#include <string>
#include <vector>
#include <thread>

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
public: 

    bool operator==(const Client& other) const {
        return name == other.name && position == other.position; 
    }

    bool completed(){
        if(direction == -2)
            return true;
        return false;
    }

    Client(string n, int speed, int& distributorDirection, const int coordinates[5]);
    string name;
    
    pair<int, int> position;

    int getIndex(const vector<Client*>& clients) const;

    void move(int& distributorDirection);
    void close();

};

#endif // CLIENT_H
