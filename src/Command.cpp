//
// Created by chenyuan on 6/26/19.
//

#include "Command.h"

Command::Command(Action action, Direction direction, Repeat repeat) :
        action_(action), direction_(direction), repeat_(repeat) {

}

std::string Command::getAction() const {
    std::string action;
    switch (action_) {
        case WALK:
            action = "walk";
            break;
        case JUMP:
            action = "jump";
            break;
        case PICK:
            action = "pick";
            break;
        case LOOP:
            action = "loop";
            break;
        default:
            action = "none";
    }
    return action;
}

std::string Command::getDirection() const {
    std::string direction;
    switch (direction_) {
        case UP:
            direction = "up";
            break;
        case DOWN:
            direction = "down";
            break;
        case LEFT:
            direction = "left";
            break;
        case RIGHT:
            direction = "right";
            break;
        default:
            direction = "none";
    }
    return direction;
}

int Command::getRepeat() const {
    return repeat_;
}


Command::~Command() = default;
