#include <Client.h>


Client::Client(std::string n, int speed, int id){
    name = n;
    position = std::make_pair(0, 10);
    this->speed = speed;
    this->id = id;
}