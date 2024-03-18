#include <Client.h>


Client::Client(std::string n, int x, int y){
    name = n;
    position = std::make_pair(x, y);
}