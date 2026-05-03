\page assumptions_page Assumptions

# Assumptions

1. The two motors act as a single door actuator.
   The current design uses `MotorPair` to command both motors together for open, close, and stop. This means that the current implementation assumes the two motors represent one physical actuator pair and should report matching motion and end states.

2. Door position is derived from the paired motor feedback.
   The two motors are assumed to remain synchronized during normal operation. If they report contradictory end states or mismatched motion, the current implementation transitions the controller into a fault state.

3. A button pressed interrupt represents a user request to toggle door motion.
   The system acknowledges the button interrupt and then forwards that event to the door controller.

4. Intermediate door position is treated as `MOVING` for control purposes.
   Any position other than fully closed (`0`) or fully open (`1023`) is handled as an in-motion or intermediate state.

5. On startup, an intermediate position is treated as a safe recovery condition that should command the door open.
   This is based directly on system requirement 7.

6. Speed selection is fixed at the current default value unless future requirements say otherwise.
   The software currently uses the motor driver's default open/close speed.

7. Register access is simulated in the development environment.
   `MockHardware` stands in for memory-mapped I/O during testing.

8. Fault handling is out of scope for the current interview implementation.
   The current solution detects basic motor disagreement and transitions to `FaultState`, but it does not implement explicit fault recovery or operator reset behavior.
