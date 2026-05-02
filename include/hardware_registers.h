#ifndef HARDWARE_REGISTERS_H
#define HARDWARE_REGISTERS_H

#include <cstdint>
#include "mock_hardware.h"

/**
 * @file hardware_registers.h
 * @brief Register addresses, offsets, masks, and register access helpers.
 */

#define MOTOR1_BASE_ADDRESS 0x80000000
#define MOTOR2_BASE_ADDRESS 0x70000000
#define BUTTON_BASE_ADDRESS 0x60000000

#define INTERRUPT_OFFSET 0x00
#define STATUS_OFFSET    0x01
#define COMMAND_OFFSET   0x02

#define MOTOR_INTERRUPT_CLOSED_MASK 0x00000001
#define MOTOR_INTERRUPT_OPEN_MASK   0x00000002

#define MOTOR_STATUS_SPEED_MASK     0x00000007
#define MOTOR_STATUS_STATE_MASK     0x00000008
#define MOTOR_STATUS_POSITION_MASK  0x00003FF0

#define MOTOR_COMMAND_COMMAND_MASK  0x00000007
#define MOTOR_COMMAND_SPEED_MASK    0x00000038
#define MOTOR_COMMAND_ENABLE_MASK   0x00000040

#define BUTTON_INTERRUPT_PRESSED_MASK  0x00000001
#define BUTTON_INTERRUPT_RELEASED_MASK 0x00000002
#define BUTTON_STATUS_STATE_MASK    0x00000001

/**
 * @brief Read a 32-bit value from a memory-mapped register.
 * @param address Register address to read.
 * @return Current register contents.
 */
inline uint32_t readRegister(uintptr_t address) {
    return MockHardware::readRegister(address);
}

/**
 * @brief Write a 32-bit value to a memory-mapped register.
 * @param address Register address to write.
 * @param value Value to store.
 */
inline void writeRegister(uintptr_t address, uint32_t value) {
    MockHardware::writeRegister(address, value);
}

#endif // HARDWARE_REGISTERS_H
