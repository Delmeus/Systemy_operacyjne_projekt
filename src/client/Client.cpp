#include <Client.h>
#include <algorithm>

Client::Client(std::string n, int speed){
    name = n;
    position = std::make_pair(0, 10);
    this->speed = speed;
}

int Client::getIndex(std::vector<Client>& clients) const {
    auto it = std::find(clients.begin(), clients.end(), *this);
    if (it != clients.end())  { 
        int index = it - clients.begin(); 
        return index;
    } 
    return -1;
}