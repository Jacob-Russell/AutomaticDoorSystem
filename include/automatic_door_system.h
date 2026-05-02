#ifndef AUTOMATIC_DOOR_SYSTEM_H
#define AUTOMATIC_DOOR_SYSTEM_H

#include "button_driver.h"
#include "door_controller.h"
#include "motor_pair.h"

/**
 * @file automatic_door_system.h
 * @brief Top-level coordinator that wires button, controller, and motor hardware flow together.
 */

/**
 * @brief System-level facade for the automatic door application.
 *
 * This class owns the major runtime collaborators and provides the external
 * event entry points used by the rest of the application:
 * - button interrupts
 * - motor interrupts
 * - explicit hardware refresh
 */
class AutomaticDoorSystem {
private:
    MotorPair motors;
    DoorController controller;
    ButtonDriver button;

public:
    /**
     * @brief Construct the door system and its owned collaborators.
     */
    AutomaticDoorSystem();

    /**
     * @brief Handle a button pressed interrupt.
     *
     * The interrupt is acknowledged in the button driver, then forwarded to the
     * stateful controller as a domain event.
     */
    void handleButtonPressedInterrupt();

    /**
     * @brief Handle a button released interrupt.
     *
     * Release currently has no domain behavior, but the interrupt is still
     * acknowledged so hardware state remains consistent.
     */
    void handleButtonReleasedInterrupt();

    /**
     * @brief Handle pending motor interrupts and refresh controller state.
     *
     * If either motor reports an open or closed interrupt, the interrupt is
     * cleared and the controller refreshes its state from motor feedback.
     */
    void handleMotorInterrupts();

    /**
     * @brief Refresh the controller state from current hardware position.
     *
     * This is useful for polling-style integration or test setup.
     */
    void refreshFromHardware();
};

#endif // AUTOMATIC_DOOR_SYSTEM_H
