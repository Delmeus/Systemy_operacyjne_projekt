#include <Client.h>
#include <algorithm>
#include <thread>
#include <iostream>
#include <ncurses.h>

Client::Client(string n, int speed, const vector<Client*>& clients){
    name = n;
    position = make_pair(0, 10);
    this->speed = speed;
    shouldClose = false;
    clientThread = thread(&Client::move, this, ref(clients));
}

void Client::move(const vector<Client*>& clients){
    int nextDirection = direction;
    while (!shouldClose){
        pair<int, int> nextPosition = position;
        /*
        Client reached station
        */
        if(position.first + 1 >= STATIONS_X){
            nextPosition.first = STATIONS_X;

            unique_lock<std::mutex> lock(clientMutex);
            condition.wait(lock,  [&]() { return !stationsOccupied[direction] || shouldClose; }); 

            if(shouldClose) break;

            position = nextPosition;
            stationsOccupied[direction] = true;
            lock.unlock();
            condition.notify_all();

            this_thread::sleep_for(chrono::seconds(3));
            
            condition.notify_all();
            lock.lock();
            position.first = STATIONS_X + 1;
            stationsOccupied[direction] = false;
            direction = -2;
            lock.unlock();
            condition.notify_all(); 

            this_thread::sleep_for(chrono::seconds(1));
            shouldClose = true;
        }      
        /*
        Client sent up, has not reached desired height
        */  
        else if(direction == 0 && position.second > TOP_STATION_Y){
            nextPosition.second -= 1;
        }
        /*
        Client sent down, has not reached desired height
        */         
        else if(direction == 2 && position.second < BOT_STATION_Y){
            nextPosition.second += 1;
        }
        else{
            nextPosition.first += 1;
        }
        /*
        Client reached the distributor
        */
        if(nextPosition.first == DISTRIBUTOR_X && nextPosition.second == DISTRIBUTOR_Y){
            unique_lock<std::mutex> lock(clientMutex);
            
            condition.wait(lock, [&]() { return !distributorOccupied || shouldClose; });

            if(shouldClose) break;
            
            distributorOccupied = true;
            position.first = DISTRIBUTOR_X;
            
            if (distributorDirection == 0){
                nextPosition.second -= 1;
                nextDirection = 0;
            }
            else if (distributorDirection == 2){
                nextPosition.second += 1;
                nextDirection = 2;
            }
            else{
                nextPosition.first += 1;
                nextDirection = 1;
            }

            lock.unlock();
            this_thread::sleep_for(chrono::seconds(1));
            lock.lock();

            position = nextPosition;
            direction = nextDirection;
            distributorOccupied = false;

            condition.notify_all(); 
        }
        else if(nextDirection != -1){
            unique_lock<std::mutex> lock(clientMutex);
            
            condition.wait(lock, [&]() { return canMove(nextPosition, clients) || shouldClose; });

            if(shouldClose) break;
            
            position = nextPosition;
            direction = nextDirection;
            
            condition.notify_all(); 
        }
        /*
         Skip locking when client is and will be still allowed to overtake in their next move
        */
        else{
            position = nextPosition;
            direction = nextDirection;
        }

        this_thread::sleep_for(chrono::milliseconds(500/speed));
        
    }
}

void Client::close(){
    shouldClose = true;
    condition.notify_all();
    if(clientThread.joinable()){
        clientThread.join();
    }
}
