# Architecture Diagrams

These diagrams are intentionally simplified. They show the main control flow and ownership boundaries without repeating every member, enum, or helper method from the code.

## 1. Class Diagram

```mermaid
classDiagram
    class AutomaticDoorSystem
    class DoorController
    class State
    class OpenState
    class ClosedState
    class MovingState
    class FaultState
    class MotorPair
    class MotorDriver
    class ButtonDriver

    AutomaticDoorSystem *-- DoorController
    AutomaticDoorSystem *-- MotorPair
    AutomaticDoorSystem *-- ButtonDriver

    DoorController --> MotorPair
    DoorController *-- State

    State <|-- OpenState
    State <|-- ClosedState
    State <|-- MovingState
    State <|-- FaultState

    MotorPair *-- MotorDriver
```

Notes:
- `AutomaticDoorSystem` is the top-level coordinator. It owns the system flow for button interrupts and motor interrupt refresh.
- `DoorController` owns the active state object and delegates button behavior to the current concrete state.
- `MotorPair` hides the two physical motors behind one actuator-facing interface.
- `MovingState` stores direction of travel internally.

## 2. State Machine Diagram

```mermaid
stateDiagram-v2
    [*] --> Closed : startup at CLOSED
    [*] --> Open : startup at OPEN
    [*] --> Moving : startup between endpoints
    [*] --> Fault : startup fault detected

    Closed --> Moving : button pressed
    Open --> Moving : button pressed

    Moving --> Open : motor refresh shows OPEN
    Moving --> Closed : motor refresh shows CLOSED
    Moving --> Fault : motor fault detected

    Closed --> Fault : motor fault detected
    Open --> Fault : motor fault detected

    Moving --> Moving : button ignored
    Fault --> Fault : button ignored
```

Notes:
- On startup, an intermediate position causes the controller to command the door open and enter `MovingState`.
- `MovingState` remembers whether the door is opening or closing, but the state machine only shows the higher-level state names.
- A motor fault means either contradictory end states or mismatched motion between the two motors.
- `FaultState` is terminal in the current design. Recovery is intentionally out of scope.

## 3. Sequence Diagrams

### Button Press While Door Is Closed

```mermaid
sequenceDiagram
    participant External as External Caller
    participant System as AutomaticDoorSystem
    participant Button as ButtonDriver
    participant Controller as DoorController
    participant Closed as ClosedState
    participant Motors as MotorPair

    External->>System: handleButtonPressedInterrupt()
    System->>Button: acknowledgePressedInterrupt()
    System->>Controller: onButtonPressed()
    Controller->>Closed: handleButtonPushed()
    Closed->>Controller: commandOpen()
    Controller->>Motors: commandOpen()
    Closed->>Controller: TransitionTo(MovingState)
```

Notes:
- `AutomaticDoorSystem` performs two separate actions: acknowledge the hardware interrupt, then notify the controller.
- `MotorPair::commandOpen()` sends the command out to both motor drivers.

### Motor Interrupt Refresh Flow

```mermaid
sequenceDiagram
    participant External as External Caller
    participant System as AutomaticDoorSystem
    participant Motors as MotorPair
    participant Controller as DoorController

    External->>System: handleMotorInterrupts()
    System->>Motors: handleInterrupts()
    Motors-->>System: interrupt handled?
    alt at least one interrupt handled
        System->>Controller: updateStateFromHardware()
        Controller->>Motors: hasFault()
        alt fault detected
            Controller->>Controller: TransitionTo(FaultState)
        else no fault
            Controller->>Motors: getPosition()
            Controller->>Controller: TransitionTo(Open/Closed/Moving)
        end
    end
```

Notes:
- `MotorPair::handleInterrupts()` clears open and closed interrupt bits for both motor drivers.
- The controller only refreshes state after the system sees that at least one motor interrupt was handled.
- The transition to `Open`, `Closed`, `Moving`, or `Fault` is decided inside `DoorController::updateStateFromHardware()`.
