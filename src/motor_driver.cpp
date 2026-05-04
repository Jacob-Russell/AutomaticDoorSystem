#include "motor_driver.h"
#include "debug.h"

MotorDriver::MotorDriver(int motorNumber) {
    
    if (motorNumber == 1) {
        this->baseAddress = MOTOR1_BASE_ADDRESS;
        LOG("[MotorDriver] Initialized Motor 1 @ 0x80000000\n");
    }
    else if (motorNumber == 2) {
        this->baseAddress = MOTOR2_BASE_ADDRESS;
        LOG("[MotorDriver] Initialized Motor 2 @ 0x70000000\n");
    }
    else {
        LOG("[MotorDriver] ERROR: Invalid motor number\n");
    }
}

int MotorDriver::getPosition() {
    // Calculate address: base + status offset
    uintptr_t statusAddress = baseAddress + STATUS_OFFSET;
    
    // Read the register
    uint32_t statusValue = readRegister(statusAddress);
    
    // Extract position bits (bits 4-13) and shift right by 4
    int position = (statusValue & MOTOR_STATUS_POSITION_MASK) >> 4;
    
    return position;
}

bool MotorDriver::isMoving() {
    uintptr_t statusAddress = baseAddress + STATUS_OFFSET;
    uint32_t statusValue = readRegister(statusAddress);
    
    // Extract STATE bit (bit 3)
    return ((statusValue & MOTOR_STATUS_STATE_MASK) >> 3) == 1;

}

int MotorDriver::getSpeed() {
    uintptr_t statusAddress = baseAddress + STATUS_OFFSET;
    uint32_t statusValue = readRegister(statusAddress);
    
    // Extract SPEED bits (bits 0-2) - no shift needed, they're at the start
    int speed = statusValue & MOTOR_STATUS_SPEED_MASK;
    
    return speed;
}

void MotorDriver::commandOpen(int speed) {
    LOG("[MotorDriver] Commanding OPEN at speed " << speed << "\n");
    
    // Build command with: OPEN (1), speed, enable (1)
    uint32_t command = buildCommand(MotorCommand::OPEN, speed, true);
    
    // Write to motor_command register
    uintptr_t commandAddress = baseAddress + COMMAND_OFFSET;
    writeRegister(commandAddress, command);
}

void MotorDriver::commandClose(int speed) {
    LOG("[MotorDriver] Commanding CLOSE at speed " << speed << "\n");
    
    // Build command with: CLOSE (0), speed, enable (1)
    uint32_t command = buildCommand(MotorCommand::CLOSE, speed, true);
    
    // Write to motor_command register
    uintptr_t commandAddress = baseAddress + COMMAND_OFFSET;
    writeRegister(commandAddress, command);
}

void MotorDriver::commandStop() {
    LOG("[MotorDriver] Commanding STOP\n");
    
    // Build command with: STOP (2), no speed, enable (1)
    uint32_t command = buildCommand(MotorCommand::STOP, 0, true);
    
    uintptr_t commandAddress = baseAddress + COMMAND_OFFSET;
    writeRegister(commandAddress, command);
}

// ============================================================================
// INTERRUPT HANDLERS
// ============================================================================

bool MotorDriver::handleOpenInterrupt() {
    if ((readInterruptRegister() & MOTOR_INTERRUPT_OPEN_MASK) == 0) {
        return false;
    }

    LOG("[MotorDriver] Clearing OPEN interrupt\n");
    clearInterruptFlag(MOTOR_INTERRUPT_OPEN_MASK);
    return true;
}

bool MotorDriver::handleClosedInterrupt() {
    if ((readInterruptRegister() & MOTOR_INTERRUPT_CLOSED_MASK) == 0) {
        return false;
    }

    LOG("[MotorDriver] Clearing CLOSED interrupt\n");
    clearInterruptFlag(MOTOR_INTERRUPT_CLOSED_MASK);
    return true;
}

// PRIVATE HELPER METHODS

uint32_t MotorDriver::buildCommand(MotorCommand cmd, int speed, bool enable) {
    uint32_t command = 0;
    
    // Set command bits (0-2)
    command |= (static_cast<uint32_t>(cmd) & 0x07);
    
    // Set speed bits (3-5)
    command |= ((speed & 0x07) << 3);
    
    // Set enable bit (6) if needed
    if (enable) {
        command |= (1 << 6);
    }
    
    return command;
}

void MotorDriver::clearInterruptFlag(uint32_t flagMask) {
    uintptr_t interruptAddress = baseAddress + INTERRUPT_OFFSET;
    
    uint32_t interruptValue = readRegister(interruptAddress);
    
    interruptValue |= flagMask;
    
    writeRegister(interruptAddress, interruptValue);
    
    LOG("[MotorDriver] Interrupt flag cleared\n");
}

uint32_t MotorDriver::readInterruptRegister() {
    return readRegister(baseAddress + INTERRUPT_OFFSET);
}
