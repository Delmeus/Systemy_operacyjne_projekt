#include <Client.h>
#include <algorithm>
#include <thread>
#include <iostream>
#include <ncurses.h>

Client::Client(string n, int speed, int& distributorDirection, const int coordinates[5], const vector<Client*>& clients, mutex& mutex, vector<bool>& occupancy, condition_variable& condition){
    name = n;
    position = make_pair(0, 10);
    this->speed = speed;
    shouldClose = false;
    copy(coordinates, coordinates + 5, stationCoordinates);
    clientThread = thread(&Client::move, this, ref(distributorDirection), ref(clients), ref(mutex), ref(occupancy), ref(condition));

}

void Client::move(int& distributorDirection, const vector<Client*>& clients, mutex& mutex, vector<bool>& occupancy, condition_variable& condition){
    int nextDirection = direction;
    while (!shouldClose){
        pair<int, int> nextPosition = position;
        /*
        Client reached station
        */
        if(position.first + 1 >= stationCoordinates[2]){
            nextPosition.first = stationCoordinates[2];

            unique_lock<std::mutex> lock(mutex);
            if (!canMove(nextPosition, clients, occupancy)) {
                condition.wait(lock,  [&]() { return canMove(nextPosition, clients, occupancy) || shouldClose; }); 
            }

            if(shouldClose) break;

            position = nextPosition;
            occupancy[direction] = true;
            lock.unlock();
            condition.notify_all();

            this_thread::sleep_for(chrono::seconds(3));
            
            condition.notify_all();
            lock.lock();
            position.first = stationCoordinates[2] + 1;
            occupancy[direction] = false;
            direction = -2;
            lock.unlock();
            condition.notify_all(); 

            this_thread::sleep_for(chrono::seconds(1));
            break;
  
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
        /*
        Client reached the distributor
        */
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
        /*
        Client sent forwards or hasn't reached the distributor yet
        */
        else{
            nextPosition.first += 1;
        }
 
        unique_lock<std::mutex> lock(mutex);
        if(!canMove(nextPosition, clients, occupancy)){
            condition.wait(lock, [&]() { return canMove(nextPosition, clients, occupancy) || shouldClose; });
        }

        if(shouldClose) break;
        
        position = nextPosition;
        direction = nextDirection;
        lock.unlock();
        condition.notify_all(); 
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