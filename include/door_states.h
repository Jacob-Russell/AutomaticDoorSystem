#ifndef DOOR_STATES_H
#define DOOR_STATES_H

#include "types.h"

class DoorController;

/**
 * @file door_states.h
 * @brief Concrete state types used by the door controller.
 */

/**
 * @brief Abstract base class for door controller states.
 *
 * Concrete states implement the response to a button press and may optionally
 * report an associated movement direction.
 */
class State {
protected:
    DoorController *context_;

public:
    virtual ~State() {}

    /**
     * @brief Supply the owning controller context to the state object.
     * @param controller Door controller that owns this state.
     */
    void set_context(DoorController *controller) {
        this->context_ = controller;
    }

    /**
     * @brief Handle a user button press while this state is active.
     */
    virtual void handleButtonPushed() = 0;

    /**
     * @brief Report movement direction associated with the current state.
     * @return `MovementDirection::NONE` unless overridden by `MovingState`.
     */
    virtual MovementDirection direction() const {
        return MovementDirection::NONE;
    }
};

/**
 * @brief State representing a fully open door.
 */
class OpenState : public State {
public:
    void handleButtonPushed() override;
};

/**
 * @brief State representing a fully closed door.
 */
class ClosedState : public State {
public:
    void handleButtonPushed() override;
};

/**
 * @brief State representing a moving door.
 *
 * MovingState stores the currently intended movement direction so the rest of
 * the system can distinguish opening motion from closing motion.
 */
class MovingState : public State {
private:
    MovementDirection direction_;

public:
    /**
     * @brief Construct a moving state with a specific movement direction.
     * @param direction Current direction of travel.
     */
    explicit MovingState(MovementDirection direction);
    void handleButtonPushed() override;
    MovementDirection direction() const override;
};

/**
 * @brief State representing a motor fault or disagreement condition.
 */
class FaultState : public State {
public:
    void handleButtonPushed() override;
};

#endif // DOOR_STATES_H
