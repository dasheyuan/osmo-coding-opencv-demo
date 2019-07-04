//
// Created by chenyuan on 6/26/19.
//

#ifndef _COMMAND_H
#define _COMMAND_H

#include <iostream>
#include <string>
#include "json.hpp"

using json = nlohmann::json;

enum Action {
    WALK, JUMP, PICK, LOOP
};
enum Direction {
    UP, DOWN, LEFT, RIGHT
};
enum Repeat {
    ONE = 1, TWO = 2, THREE = 3, FOUR = 4, FIVE = 5
};

class Command {
private:
    Action action_;
    Direction direction_;
    Repeat repeat_;
public:
    Command(Action action, Direction direction, Repeat repeat);

    ~Command();

    std::string getAction() const;

    std::string getDirection() const;

    int getRepeat() const;

    json getCommand() {
        json cmd;
        cmd["action"] = getAction();
        if (getAction() != "loop")
            cmd["direction"] = getDirection();
        cmd["repeat"] = getRepeat();
        return cmd;
    }
};


#endif //_COMMAND_H
