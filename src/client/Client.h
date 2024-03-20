#ifndef CLIENT_H
#define CLIENT_H

#include <utility>
#include <string>

class Client {
public:

    Client(std::string n, int x, int y, int speed);

public:
    std::string name;
    std::pair<int, int> position;
    int speed;
};

#endif // CLIENT_H
