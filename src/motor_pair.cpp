#include "motor_pair.h"
#include "types.h"

MotorPair::MotorPair() : motor1(1), motor2(2) {}

int MotorPair::getPosition() {
    const int position1 = motor1.getPosition();
    const int position2 = motor2.getPosition();

    if (position1 == static_cast<int>(DoorPosition::OPEN) &&
        position2 == static_cast<int>(DoorPosition::OPEN)) {
        return static_cast<int>(DoorPosition::OPEN);
    }

    if (position1 == static_cast<int>(DoorPosition::CLOSED) &&
        position2 == static_cast<int>(DoorPosition::CLOSED)) {
        return static_cast<int>(DoorPosition::CLOSED);
    }

    return static_cast<int>((position1 + position2) / 2);
}

bool MotorPair::isMoving() {
    return motor1.isMoving() || motor2.isMoving();
}

bool MotorPair::hasFault() {
    const int position1 = motor1.getPosition();
    const int position2 = motor2.getPosition();
    const bool moving1 = motor1.isMoving();
    const bool moving2 = motor2.isMoving();

    const bool contradictoryEndStates =
        (position1 == static_cast<int>(DoorPosition::OPEN) &&
         position2 == static_cast<int>(DoorPosition::CLOSED)) ||
        (position1 == static_cast<int>(DoorPosition::CLOSED) &&
         position2 == static_cast<int>(DoorPosition::OPEN));

    const bool mismatchedMotion = moving1 != moving2;

    return contradictoryEndStates || mismatchedMotion;
}

bool MotorPair::handleInterrupts() {
    bool handled = false;

    handled = motor1.handleOpenInterrupt() || handled;
    handled = motor1.handleClosedInterrupt() || handled;
    handled = motor2.handleOpenInterrupt() || handled;
    handled = motor2.handleClosedInterrupt() || handled;

    return handled;
}

void MotorPair::commandOpen(int speed) {
    motor1.commandOpen(speed);
    motor2.commandOpen(speed);
}

void MotorPair::commandClose(int speed) {
    motor1.commandClose(speed);
    motor2.commandClose(speed);
}

void MotorPair::commandStop() {
    motor1.commandStop();
    motor2.commandStop();
}
