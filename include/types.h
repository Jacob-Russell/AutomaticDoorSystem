#ifndef TYPES_H
#define TYPES_H

/**
 * @file types.h
 * @brief Shared enum types used by the automatic door application.
 */

/**
 * @brief Door endpoint positions defined by the customer memo.
 */
enum class DoorPosition {
  CLOSED = 0,
  OPEN = 1023
};

/**
 * @brief Direction of travel while the door is moving.
 */
enum class MovementDirection {
  NONE = 0,
  OPENING,
  CLOSING
};

/**
 * @brief Commands supported by the motor command register.
 */
enum class MotorCommand {
  CLOSE = 0,
  OPEN = 1,
  STOP = 2
};

#endif // TYPES_H
