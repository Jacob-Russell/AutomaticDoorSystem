#include "door_states.h"
#include "door_controller.h"
#include <iostream>

void OpenState::handleButtonPushed() {
    std::cout << "[OpenState] Button -> command CLOSE\n";
    this->context_->commandClose();
    this->context_->TransitionTo(new MovingState(MovementDirection::CLOSING));
}

void ClosedState::handleButtonPushed() {
    std::cout << "[ClosedState] Button -> command OPEN\n";
    this->context_->commandOpen();
    this->context_->TransitionTo(new MovingState(MovementDirection::OPENING));
}

MovingState::MovingState(MovementDirection direction) : direction_(direction) {}

void MovingState::handleButtonPushed() {
    std::cout << "[MovingState] Button ignored while MOVING\n";
}

MovementDirection MovingState::direction() const {
    return direction_;
}

void FaultState::handleButtonPushed() {
    std::cout << "[FaultState] Button ignored while FAULTED\n";
}
