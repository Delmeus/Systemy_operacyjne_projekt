#ifndef CLIENT_H
#define CLIENT_H

#include <utility>
#include <string>

class Client {
public:

    Client(std::string n, int speed, int id);

public:
    std::string name;
    std::pair<int, int> position;
    int speed;
    int direction = -1;
    int id;
};

#endif // CLIENT_H
