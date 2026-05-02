#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include "hardware_registers.h"
#include "types.h"

/**
 * @file motor_driver.h
 * @brief Memory-mapped I/O wrapper for a single motor device.
 */

/**
 * @brief Driver for one motor hardware instance.
 *
 * The driver exposes position/status reads, command writes, and interrupt
 * acknowledgment helpers for one memory-mapped motor.
 */
class MotorDriver {
private:
    uintptr_t baseAddress;

    uint32_t buildCommand(MotorCommand cmd, int speed, bool enable);
    void clearInterruptFlag(uint32_t flagMask);
    uint32_t readInterruptRegister();

public:
    /**
     * @brief Construct a motor driver for one of the supported motor instances.
     * @param motorNumber Supported values are `1` and `2`.
     */
    explicit MotorDriver(int motorNumber);
    ~MotorDriver() {}

    /**
     * @brief Read the current motor position from hardware.
     * @return Position in the range described by the customer memo.
     */
    int getPosition();

    /**
     * @brief Read whether the motor currently reports motion.
     * @return true if the motor status register reports MOVING.
     */
    bool isMoving();

    /**
     * @brief Read the current motor speed from hardware.
     * @return Instantaneous speed value from the motor status register.
     */
    int getSpeed();

    /**
     * @brief Command the motor to move toward the open position.
     * @param speed Requested motor speed.
     */
    void commandOpen(int speed = 4);

    /**
     * @brief Command the motor to move toward the closed position.
     * @param speed Requested motor speed.
     */
    void commandClose(int speed = 4);

    /**
     * @brief Command the motor to stop.
     */
    void commandStop();

    /**
     * @brief Acknowledge an open-position interrupt if present.
     * @return true if an interrupt bit was present and handled.
     */
    bool handleOpenInterrupt();

    /**
     * @brief Acknowledge a closed-position interrupt if present.
     * @return true if an interrupt bit was present and handled.
     */
    bool handleClosedInterrupt();
};

#endif // MOTOR_DRIVER_H
