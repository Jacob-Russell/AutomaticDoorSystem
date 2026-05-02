## To view full documentation please visit: 
https://jacob-russell.github.io/AutomaticDoorSystem/

# AutomaticDoorSystem
C++ interview implementation of a 2-motor, single push button, automatic door operating system

## Software Requirements

This document traces the implemented software behavior back to the seven core system requirements from `ProjectContext.txt`.

1. When the button is pressed while the door is open, the system shall command the door to close.
   Implemented through `OpenState`, which issues a close command and transitions the controller to `MovingState`.

2. When the button is pressed while the door is closed, the system shall command the door to open.
   Implemented through `ClosedState`, which issues an open command and transitions the controller to `MovingState`.

3. When the button is pressed while the door is moving, the system shall ignore the request.
   Implemented through `MovingState`, which performs no motor command on button input.

4. The system shall monitor door position.
   Implemented through the motor layer and controller state refresh logic. The controller derives `OPEN`, `CLOSED`, or `MOVING` from the reported motor position.

5. The system shall be able to command the motor(s) to move to the open position.
   Implemented through `MotorPair::commandOpen()`, which forwards the open command to both motors.

6. The system shall be able to command the motor(s) to move to the close position.
   Implemented through `MotorPair::commandClose()`, which forwards the close command to both motors.

7. At initialization, if the door is not fully open or fully closed, the system shall command the door open.
   Implemented in `DoorController` initialization logic. If the startup position is intermediate, the controller commands open.

### Implementation Notes

1. The current implementation uses a classic State pattern to model `OPEN`, `CLOSED`, and `MOVING`.
   A `FaultState` is also present to contain basic motor disagreement behavior.

2. The current implementation treats the two motors as one paired actuator.

3. The current implementation assumes both motors remain synchronized during normal operation. If they do not, contradictory end states or mismatched motion place the controller into `FaultState`.


## Proposed Refinements

These items are not required to satisfy the current interview problem, but they are reasonable discussion points for future refinement.

1. Clarify dual-motor behavior.
   The customer memo lists two motors, but does not say whether one motor may continue alone or whether both positions must always match.

2. Define fault behavior.
   The current requirements do not describe what the system should do after the controller enters `FaultState` due to motor disagreement or mismatched motion.

3. Clarify startup behavior after a forced open command.
   Requirement 7 says to command open at initialization from an intermediate state, but does not specify whether additional verification or timeout behavior is required.

4. Clarify interrupt ownership and control flow expectations.
   The current design uses a small system-level coordinator to acknowledge hardware interrupts and then dispatch domain behavior. This is a reasonable architecture choice, but the customer memo does not prescribe whether that flow should be polling-based, interrupt-driven, or event-loop based.

5. Define whether button release has any behavioral meaning.
   The current implementation acknowledges release interrupts but does not use them for door control because no requirement currently depends on button release.

6. Define expected response to motor disagreement.
   The current implementation transitions to `FaultState` when the motors report contradictory end states or mismatched motion. A future refinement could define whether disagreement should trigger degraded mode, fault handling, or continued operation on one motor.

7. Define acceptable command speed behavior.
   The current implementation uses a fixed default speed. A future revision could make speed configurable if required by product variants.

8. Add explicit requirements for safety and recovery.
   For a production-quality system, future requirements might include stop behavior, timeout behavior, retry policy, and degraded-mode handling.

9. Currently, motor disagreement drives the controller into `FaultState`, where button input is ignored and no recovery path is provided. A future enhancement would define whether disagreement should require operator reset, whether one motor may continue alone, and how the software should behave if a failure occurs while the door is fully open or fully closed.


## Assumptions

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
