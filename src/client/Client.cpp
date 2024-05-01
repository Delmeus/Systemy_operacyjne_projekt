#include <Client.h>
#include <algorithm>
#include <thread>
#include <iostream>
#include <ncurses.h>

Client::Client(string n, int speed, int& distributorDirection, const int coordinates[5], const vector<Client*>& clients, mutex& mutex, vector<bool>& occupancy){
    name = n;
    position = make_pair(0, 10);
    this->speed = speed;
    shouldClose = false;
    copy(coordinates, coordinates + 5, stationCoordinates);
    clientThread = thread(&Client::move, this, ref(distributorDirection), ref(clients), ref(mutex), ref(occupancy));

}

int Client::getIndex(const vector<Client*>& clients) const {
    auto it = find_if(clients.begin(), clients.end(), [&](const Client* client) {
        return *client == *this;
    });
    if (it != clients.end()) {
        return distance(clients.begin(), it);
    }
    return -1;
}

void Client::move(int& distributorDirection, const vector<Client*>& clients, mutex& mutex, vector<bool>& occupancy){
    int nextDirection = direction;
    while (!shouldClose){
        pair<int, int> nextPosition = position;
        if(position.first + 1 >= stationCoordinates[2]){
            nextPosition.first = stationCoordinates[2];

            mutex.lock();
            if(canMove(nextPosition, clients, occupancy)){
                position = nextPosition;
                occupancy[direction] = true;
                mutex.unlock();
                this_thread::sleep_for(chrono::seconds(3));

                mutex.lock();
                position.first = stationCoordinates[2] + 1;
                occupancy[direction] = false;
                mutex.unlock();
                this_thread::sleep_for(chrono::seconds(1));
                direction = -2;
                break;
            }
            mutex.unlock();
        }      
        /*
        Client sent up
        */  
        else if(direction == 0){
            if(position.second > stationCoordinates[3]){
                if(position.second - 1 <= stationCoordinates[3]){
                    nextPosition.second = stationCoordinates[3];
                }
                else{
                    nextPosition.second -= 1;
                }
            }
            else{
                nextPosition.first += 1;
            }
        }
        /*
        Client sent down
        */         
        else if(direction == 2){
            if(position.second < stationCoordinates[4]){
                if(position.second + 1 >= stationCoordinates[4]){
                    nextPosition.second = stationCoordinates[4];
                }
                else{
                    nextPosition.second += 1;
                }
            }
            else{
                nextPosition.first += 1;
            }
        }
        else if (position.first + 1 >= stationCoordinates[0] && direction == -1){
            if (distributorDirection == 0){
                nextPosition = make_pair(stationCoordinates[0], stationCoordinates[1] - 1);
                nextDirection = 0;
            }
            else if (distributorDirection == 2){
                nextPosition = make_pair(stationCoordinates[0], stationCoordinates[1] + 1);
                nextDirection = 2;
            }
            else{
                nextPosition = make_pair(stationCoordinates[0] + 1, stationCoordinates[1]);
                nextDirection = 1;
            }
        }
        else{
            nextPosition.first += 1;
        }

        mutex.lock();
        if(canMove(nextPosition, clients, occupancy)){
            position = nextPosition;
            direction = nextDirection;
            mutex.unlock();
            this_thread::sleep_for(chrono::milliseconds(500/speed));
        }
        else{
            mutex.unlock();
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }
}

void Client::close(){
    shouldClose = true;
    clientThread.join();
}