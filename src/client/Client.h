#ifndef CLIENT_H
#define CLIENT_H

#include <utility>
#include <string>
#include <vector>

class Client {
public:

    bool operator==(const Client& other) const {
        return name == other.name && position == other.position; 
    }

    Client(std::string n, int speed);
    std::string name;
    std::pair<int, int> position;
    int speed;
    int direction = -1;
    int getIndex(std::vector<Client>& clients) const;

};

#endif // CLIENT_H
