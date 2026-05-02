#ifndef DOOR_CONTROLLER_H
#define DOOR_CONTROLLER_H

#include <memory>
#include <typeinfo>
#include "door_states.h"
#include "types.h"

class MotorPair;

/**
 * @file door_controller.h
 * @brief State-pattern context for the automatic door control logic.
 */

/**
 * @brief Domain controller for door behavior.
 *
 * DoorController owns the active state object and delegates button input to the
 * current concrete state. It also translates hardware position feedback into
 * high-level controller state transitions.
 */
class DoorController {
private:
    MotorPair &motor;
    std::unique_ptr<State> state_;
    MovementDirection lastDirection_;

    void setStateFromPosition(int position);

public:
    /**
     * @brief Construct a controller bound to the paired-motor actuator.
     * @param motor Paired motor actuator used for position feedback and commands.
     */
    explicit DoorController(MotorPair &motor);
    ~DoorController() = default;

    /**
     * @brief Replace the active state object.
     * @param nextState Newly allocated concrete state owned by the controller.
     */
    void TransitionTo(State *nextState);

    /**
     * @brief Forward a button event to the active door state.
     */
    void onButtonPressed();

    /**
     * @brief Refresh controller state from motor feedback.
     */
    void updateStateFromHardware();

    /**
     * @brief Command the actuator pair to open the door.
     */
    void commandOpen();

    /**
     * @brief Command the actuator pair to close the door.
     */
    void commandClose();

    /**
     * @brief Report the currently tracked movement direction while moving.
     * @return The last commanded movement direction, or `NONE` when stationary.
     */
    MovementDirection currentDirection() const;
};

#endif // DOOR_CONTROLLER_H
