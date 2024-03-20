#include <Client.h>


Client::Client(std::string n, int x, int y, int speed){
    name = n;
    position = std::make_pair(x, y);
    this->speed = speed;
}