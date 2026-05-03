\page requirements_page Software Requirements

# Software Requirements

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

## Implementation Notes

1. The current implementation uses a classic State pattern to model `OPEN`, `CLOSED`, and `MOVING`.
   A `FaultState` is also present to contain basic motor disagreement behavior.

2. The current implementation treats the two motors as one paired actuator.

3. The current implementation assumes both motors remain synchronized during normal operation. If they do not, contradictory end states or mismatched motion place the controller into `FaultState`.
