#ifndef BUTTON_DRIVER_H
#define BUTTON_DRIVER_H

#include "hardware_registers.h"

/**
 * @file button_driver.h
 * @brief Memory-mapped I/O wrapper for the push button device.
 */

/**
 * @brief Driver for button status and interrupt acknowledgment.
 *
 * This class is intentionally narrow: it reads button state and services button
 * interrupts, but it does not contain any door-control policy.
 */
class ButtonDriver {
private:
    uintptr_t baseAddress = BUTTON_BASE_ADDRESS;

public:
    ButtonDriver() = default;
    ~ButtonDriver() {}

    /**
     * @brief Read the current pressed/released state from hardware.
     * @return true if the button is currently pressed.
     */
    bool isPressed();

    /**
     * @brief Acknowledge a pressed interrupt in the button device.
     */
    void acknowledgePressedInterrupt();
    
    /**
     * @brief Acknowledge a released interrupt in the button device.
     */
    void acknowledgeReleasedInterrupt();

private:
    /**
     * @brief Clear a button interrupt bit in the memory-mapped interrupt register.
     * @param flagMask Bit mask identifying the interrupt to clear.
     */
    void clearInterruptFlag(uint32_t flagMask);
};

#endif // BUTTON_DRIVER_H
