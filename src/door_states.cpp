#include "door_states.h"
#include "door_controller.h"
#include "debug.h"

void OpenState::handleButtonPushed() {
    LOG("[OpenState] Button -> command CLOSE\n");
    this->context_->commandClose();
    this->context_->TransitionTo(new MovingState(MovementDirection::CLOSING));
}

void ClosedState::handleButtonPushed() {
    LOG("[ClosedState] Button -> command OPEN\n");
    this->context_->commandOpen();
    this->context_->TransitionTo(new MovingState(MovementDirection::OPENING));
}

MovingState::MovingState(MovementDirection direction) : direction_(direction) {}

void MovingState::handleButtonPushed() {
    LOG("[MovingState] Button ignored while MOVING\n");
}

MovementDirection MovingState::direction() const {
    return direction_;
}

void FaultState::handleButtonPushed() {
    LOG("[FaultState] Button ignored while FAULTED\n");
}
