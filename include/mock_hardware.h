#ifndef MOCK_HARDWARE_H
#define MOCK_HARDWARE_H

#include <cstdint>
#include <map>

/**
 * @file mock_hardware.h
 * @brief In-memory mock for memory-mapped hardware registers.
 */

/**
 * @brief Test double for the platform's memory-mapped register space.
 */
class MockHardware {
private:
    static std::map<uintptr_t, uint32_t> registers;

public:
    /**
     * @brief Write a value into the mock register map.
     * @param address Register address.
     * @param value Value to write.
     */
    static void writeRegister(uintptr_t address, uint32_t value);

    /**
     * @brief Read a value from the mock register map.
     * @param address Register address.
     * @return Register contents, defaulting to zero when not present.
     */
    static uint32_t readRegister(uintptr_t address);

    /**
     * @brief Reset the mock register map to an empty state.
     */
    static void reset();
};

#endif
