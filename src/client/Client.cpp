#include <Client.h>
#include <algorithm>
#include <thread>
#include <iostream>
#include <ncurses.h>

Client::Client(string n, int speed, int& distributorDirection, const int coordinates[5], const vector<Client*>& clients, mutex& mutex, vector<bool>& stationsOccupied, condition_variable& condition){
    name = n;
    position = make_pair(0, 10);
    this->speed = speed;
    shouldClose = false;
    copy(coordinates, coordinates + 5, stationCoordinates);
    clientThread = thread(&Client::move, this, ref(distributorDirection), ref(clients), ref(mutex), ref(stationsOccupied), ref(condition));

}

void Client::move(int& distributorDirection, const vector<Client*>& clients, mutex& mutex, vector<bool>& stationsOccupied, condition_variable& condition){
    int nextDirection = direction;
    while (!shouldClose){
        pair<int, int> nextPosition = position;
        /*
        Client reached station
        */
        if(position.first + 1 >= stationCoordinates[2]){
            nextPosition.first = stationCoordinates[2];

            unique_lock<std::mutex> lock(mutex);
            condition.wait(lock,  [&]() { return !stationsOccupied[direction] || shouldClose; }); 

            if(shouldClose) break;

            position = nextPosition;
            stationsOccupied[direction] = true;
            lock.unlock();
            condition.notify_all();

            this_thread::sleep_for(chrono::seconds(3));
            
            condition.notify_all();
            lock.lock();
            position.first = stationCoordinates[2] + 1;
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
        else if(direction == 0 && position.second > stationCoordinates[3]){
            nextPosition.second -= 1;
        }
        /*
        Client sent down, has not reached desired height
        */         
        else if(direction == 2 && position.second < stationCoordinates[4]){
            nextPosition.second += 1;
        }
        else{
            nextPosition.first += 1;
        }
        /*
        Client reached the distributor
        */
        if(nextPosition.first == stationCoordinates[0] && nextPosition.second == stationCoordinates[1]){
            unique_lock<std::mutex> lock(mutex);
            
            condition.wait(lock, [&]() { return !distributorOccupied || shouldClose; });

            if(shouldClose) break;
            
            distributorOccupied = true;
            position.first = stationCoordinates[0];
            
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
            unique_lock<std::mutex> lock(mutex);
            
            condition.wait(lock, [&]() { return canMove(nextPosition, clients, stationsOccupied) || shouldClose; });

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

void Client::close(condition_variable& condition){
    shouldClose = true;
    condition.notify_all();
    if(clientThread.joinable()){
        clientThread.join();
    }
}
