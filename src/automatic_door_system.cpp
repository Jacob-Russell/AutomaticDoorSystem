#include "automatic_door_system.h"

AutomaticDoorSystem::AutomaticDoorSystem()
    : motors(), controller(motors), button() {}

void AutomaticDoorSystem::handleButtonPressedInterrupt() {
    button.acknowledgePressedInterrupt();
    controller.onButtonPressed();
}

void AutomaticDoorSystem::handleButtonReleasedInterrupt() {
    button.acknowledgeReleasedInterrupt();
}

void AutomaticDoorSystem::handleMotorInterrupts() {
    if (motors.handleInterrupts()) {
        controller.updateStateFromHardware();
    }
}

void AutomaticDoorSystem::refreshFromHardware() {
    controller.updateStateFromHardware();
}
