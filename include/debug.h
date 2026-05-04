#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>

/**
 * @file types.h
 * @brief Shared enum types used by the automatic door application.
 */

 /**
 * @brief Conditional diagnostic logging macro.
 *
 * Compile with -DDEBUG to enable log output. In production builds this macro
 * expands to nothing, producing no code and no runtime overhead. To redirect
 * output to a platform logger, replace the std::cout line here without touching
 * any call site.
 *
 * Usage: LOG("[MotorDriver] Position: " << position << "\n");
 */
#ifdef DEBUG
    #define LOG(msg) std::cout << msg
#else
    #define LOG(msg) do {} while(0)
#endif

#endif // DEBUG_H