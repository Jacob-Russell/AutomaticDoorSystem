#include "button_driver.h"
#include "debug.h"

bool ButtonDriver::isPressed() {
    uintptr_t statusAddress = baseAddress + STATUS_OFFSET;
    uint32_t statusValue = readRegister(statusAddress);
    
    // Extract STATE bit (bit 0)
    // 0 = RELEASED, 1 = PRESSED
    bool state = (statusValue & BUTTON_STATUS_STATE_MASK);
    
    return state == 1;
}

void ButtonDriver::acknowledgePressedInterrupt() {
    LOG("[ButtonDriver] Interrupt: Button PRESSED\n");
    clearInterruptFlag(BUTTON_INTERRUPT_PRESSED_MASK);
}

void ButtonDriver::acknowledgeReleasedInterrupt() {
    LOG("[ButtonDriver] Interrupt: Button RELEASED\n");
    
    // Clear interrupt flag
    clearInterruptFlag(BUTTON_INTERRUPT_RELEASED_MASK);
}

void ButtonDriver::clearInterruptFlag(uint32_t flagMask) {
    uintptr_t interruptAddress = baseAddress + INTERRUPT_OFFSET;
    
    uint32_t interruptValue = readRegister(interruptAddress);
    interruptValue |= flagMask;
    writeRegister(interruptAddress, interruptValue);
    
    LOG("[ButtonDriver] Interrupt flag cleared\n");
}
