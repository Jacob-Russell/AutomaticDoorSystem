#ifndef MOTOR_PAIR_H
#define MOTOR_PAIR_H

#include "motor_driver.h"

/**
 * @file motor_pair.h
 * @brief Paired actuator abstraction built on top of two motor drivers.
 */

/**
 * @brief Coordinates the two drive motors as one door actuator.
 *
 * MotorPair centralizes paired reads, paired commands, and simple disagreement
 * detection so the controller can reason about a single actuator.
 */
class MotorPair {
private:
    MotorDriver motor1;
    MotorDriver motor2;

public:
    /**
     * @brief Construct the paired-motor actuator wrapper.
     */
    MotorPair();

    /**
     * @brief Derive a single door position from both motors.
     * @return Fully open, fully closed, or an intermediate position.
     */
    int getPosition();

    /**
     * @brief Report whether either motor currently indicates motion.
     * @return true if either motor is moving.
     */
    bool isMoving();

    /**
     * @brief Detect basic disagreement between the two motors.
     * @return true when the motors report contradictory end states or mismatched motion.
     */
    bool hasFault();

    /**
     * @brief Acknowledge any pending open/closed motor interrupts.
     * @return true if at least one interrupt was handled.
     */
    bool handleInterrupts();

    /**
     * @brief Command both motors to move toward the open position.
     * @param speed Requested motor speed.
     */
    void commandOpen(int speed = 4);

    /**
     * @brief Command both motors to move toward the closed position.
     * @param speed Requested motor speed.
     */
    void commandClose(int speed = 4);

    /**
     * @brief Command both motors to stop.
     */
    void commandStop();
};

#endif // MOTOR_PAIR_H
