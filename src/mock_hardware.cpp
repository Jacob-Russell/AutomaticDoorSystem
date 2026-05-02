#include "mock_hardware.h"

std::map<uintptr_t, uint32_t> MockHardware::registers;

void MockHardware::writeRegister(uintptr_t address, uint32_t value) {
    registers[address] = value;
}

uint32_t MockHardware::readRegister(uintptr_t address) {
    if (registers.find(address) == registers.end()) {
        registers[address] = 0;
    }
    return registers[address];
}

void MockHardware::reset() {
    registers.clear();
}