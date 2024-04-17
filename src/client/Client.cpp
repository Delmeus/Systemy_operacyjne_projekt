#include <Client.h>
#include <algorithm>
#include <thread>
#include <iostream>
#include <ncurses.h>
Client::Client(string n, int speed, int& distributorDirection, const int coordinates[5]){
    name = n;
    position = make_pair(0, 10);
    this->speed = speed;
    shouldClose = false;
    copy(coordinates, coordinates + 5, stationCoordinates);
    clientThread = thread(&Client::move, this, ref(distributorDirection));

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

void Client::move(int& distributorDirection){
    //int speed = 1;
    while (!shouldClose){
        if(position.first + 1 >= stationCoordinates[2]){
            position.first = stationCoordinates[2];
            this_thread::sleep_for(chrono::seconds(3));
            position.first = stationCoordinates[2] + 1;
            this_thread::sleep_for(chrono::seconds(1));
            direction = -2;
            break;
        }      
        /*
         Client sent up
        */  
        else if(direction == 0){
            if(position.second > stationCoordinates[3]){
                if(position.second - 1 <= stationCoordinates[3]){
                    position.second = stationCoordinates[3];
                }
                else{
                    position.second -= 1;
                }
            }
            else{
                position.first += 1;
            }
        }
        /*
         Client sent down
        */         
        else if(direction == 2){
            if(position.second < stationCoordinates[4]){
                if(position.second + 1 >= stationCoordinates[4]){
                    position.second = stationCoordinates[4];
                }
                else{
                    position.second += 1;
                }
            }
            else{
                position.first += 1;
            }
        }
        else if (position.first + 1 >= stationCoordinates[0] && direction == -1){
            if (distributorDirection == 0){
                position = make_pair(stationCoordinates[0], stationCoordinates[1] - 1);
                direction = 0;
            }
            else if (distributorDirection == 2){
                position = make_pair(stationCoordinates[0], stationCoordinates[1] + 1);
                direction = 2;
            }
            else{
                position = make_pair(stationCoordinates[0] + 1, stationCoordinates[1]);
                direction = 1;
            }
        }
        else if((position.first + 1 < stationCoordinates[0] && direction == -1) || direction == 1){
            position.first += 1;
        }
        else{
            position.first += 1;
        }
        this_thread::sleep_for(chrono::milliseconds(500/speed));
    }
}

void Client::close(){
    shouldClose = true;
    clientThread.join();
}