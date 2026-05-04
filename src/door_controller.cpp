#include "door_controller.h"
#include "motor_pair.h"
#include "debug.h"

DoorController::DoorController(MotorPair &motor)
    : motor(motor), state_(nullptr), lastDirection_(MovementDirection::NONE) {
    updateStateFromHardware();

    const int initialPosition = motor.getPosition();
    if (!motor.hasFault() &&
        initialPosition != static_cast<int>(DoorPosition::OPEN) &&
        initialPosition != static_cast<int>(DoorPosition::CLOSED)) {
        LOG("[DoorController] Init: Intermediate position - forcing OPEN\n");
        motor.commandOpen();
        TransitionTo(new MovingState(MovementDirection::OPENING));
    }
}

void DoorController::setStateFromPosition(int position) {
    if (motor.hasFault()) {
        TransitionTo(new FaultState);
        return;
    }

    if (position >= static_cast<int>(DoorPosition::OPEN)) {
        lastDirection_ = MovementDirection::NONE;
        TransitionTo(new OpenState);
    }
    else if (position <= static_cast<int>(DoorPosition::CLOSED)) {
        lastDirection_ = MovementDirection::NONE;
        TransitionTo(new ClosedState);
    }
    else {
        TransitionTo(new MovingState(lastDirection_));
    }
}

void DoorController::TransitionTo(State *nextState) {
    LOG("[DoorController] Transition to " << typeid(*nextState).name() << "\n");
    this->state_.reset(nextState);
    this->state_->set_context(this);
}

void DoorController::onButtonPressed() {
    if (this->state_ != nullptr) {
        this->state_->handleButtonPushed();
    }
}

void DoorController::updateStateFromHardware() {
    if (motor.hasFault()) {
        TransitionTo(new FaultState);
        return;
    }

    setStateFromPosition(motor.getPosition());
}

void DoorController::commandOpen() {
    lastDirection_ = MovementDirection::OPENING;
    motor.commandOpen();
}

void DoorController::commandClose() {
    lastDirection_ = MovementDirection::CLOSING;
    motor.commandClose();
}

MovementDirection DoorController::currentDirection() const {
    if (state_ == nullptr) {
        return MovementDirection::NONE;
    }

    return state_->direction();
}
