# Operative systems - project

## Goal

The goal of this project is writing a multi-thread application in C++ using _pthreads_. 

## Topic

There are three stages to this project.

### Stage 1

Clients appear at random moments in time on the beginning of the corridor. They move at constant speed towards distributor. Distributor points at one of three service stations, alternating between them with fixed period. When client reaches the distributor, they are send towards one of the three stations (the one where distributor is currently pointing). From this point client moves until they reach the service station, where they stop for few seconds and then they leave the system. 

To visualize behaviour of clients and distributor I used the _ncurses_ library. Every client is represented by a letter.

### Stage 2

Clients cannot pass each other on their way from distributor to service station. Additionally, when station is occupied, they cannot enter, forming a group in cell before it.

### Stage 3

Distributor now requires 1 second for a client to pass. Upon reaching the distributor client blocks it for other clients. After 1 second client leaves into direction that was pointed by the distributor, at the moment in which they entered it.

