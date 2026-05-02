# Architecture Diagrams

This document contains the core architecture diagrams for the Automatic Door Opener interview implementation.

Recommended submission set:

1. Class Diagram
2. State Machine Diagram
3. Sequence Diagram

These three diagrams are enough to show structure, runtime behavior, and the role of the State pattern without overwhelming the reviewer.

## 1. Class Diagram

```mermaid
classDiagram
    class AutomaticDoorSystem {
        -MotorPair motors
        -DoorController controller
        -ButtonDriver button
        +handleButtonPressedInterrupt()
        +handleButtonReleasedInterrupt()
        +handleMotorInterrupts()
        +refreshFromHardware()
    }

    class DoorController {
        -MotorPair& motor
        -unique_ptr~State~ state_
        -MovementDirection lastDirection_
        -setStateFromPosition(position)
        +TransitionTo(State*)
        +onButtonPressed()
        +updateStateFromHardware()
        +commandOpen()
        +commandClose()
        +currentDirection()
    }

    class State {
        <<abstract>>
        #DoorController* context_
        +set_context(controller)
        +handleButtonPushed()*
        +direction()
    }

    class OpenState {
        +handleButtonPushed()
    }

    class ClosedState {
        +handleButtonPushed()
    }

    class MovingState {
        -MovementDirection direction_
        +MovingState(direction)
        +handleButtonPushed()
        +direction()
    }

    class FaultState {
        +handleButtonPushed()
    }

    class MotorPair {
        -MotorDriver motor1
        -MotorDriver motor2
        +getPosition()
        +isMoving()
        +hasFault()
        +handleInterrupts()
        +commandOpen(speed)
        +commandClose(speed)
        +commandStop()
    }

    class MotorDriver {
        -uintptr_t baseAddress
        +getPosition()
        +isMoving()
        +getSpeed()
        +commandOpen(speed)
        +commandClose(speed)
        +commandStop()
        +handleOpenInterrupt()
        +handleClosedInterrupt()
    }

    class ButtonDriver {
        -uintptr_t baseAddress
        +isPressed()
        +acknowledgePressedInterrupt()
        +acknowledgeReleasedInterrupt()
    }

    class MockHardware {
        <<static>>
        +writeRegister(address, value)
        +readRegister(address)
        +reset()
    }

    class DoorPosition {
        <<enumeration>>
        CLOSED
        OPEN
    }

    class MovementDirection {
        <<enumeration>>
        NONE
        OPENING
        CLOSING
    }

    class MotorCommand {
        <<enumeration>>
        CLOSE
        OPEN
        STOP
    }

    AutomaticDoorSystem *-- MotorPair
    AutomaticDoorSystem *-- DoorController
    AutomaticDoorSystem *-- ButtonDriver

    DoorController --> MotorPair
    DoorController *-- State

    State <|-- OpenState
    State <|-- ClosedState
    State <|-- MovingState
    State <|-- FaultState

    MotorPair *-- MotorDriver
    MotorDriver ..> MockHardware
    ButtonDriver ..> MockHardware

    DoorController ..> MovementDirection
    MovingState ..> MovementDirection
    MotorDriver ..> MotorCommand
    MotorPair ..> DoorPosition
```

## 2. State Machine Diagram

```mermaid
stateDiagram-v2
    [*] --> Closed : startup at position 0
    [*] --> Open : startup at position 1023
    [*] --> Moving : startup at intermediate position
    [*] --> Fault : startup with motor disagreement

    Closed --> Moving : button pressed / commandOpen()
    Open --> Moving : button pressed / commandClose()
    Moving --> Moving : button pressed ignored
    Fault --> Fault : button pressed ignored

    Moving --> Open : motor interrupts + position == OPEN
    Moving --> Closed : motor interrupts + position == CLOSED
    Moving --> Fault : contradictory end states\nor mismatched motion

    Closed --> Fault : motor disagreement detected
    Open --> Fault : motor disagreement detected

    note right of Moving
      MovingState stores direction:
      OPENING or CLOSING
    end note
```

## 3. Sequence Diagram

### Button Press While Door Is Closed

```mermaid
sequenceDiagram
    participant User
    participant Button as ButtonDriver
    participant System as AutomaticDoorSystem
    participant Controller as DoorController
    participant State as ClosedState
    participant Motors as MotorPair
    participant M1 as MotorDriver(1)
    participant M2 as MotorDriver(2)

    User->>System: handleButtonPressedInterrupt()
    System->>Button: acknowledgePressedInterrupt()
    System->>Controller: onButtonPressed()
    Controller->>State: handleButtonPushed()
    State->>Controller: commandOpen()
    Controller->>Motors: commandOpen()
    Motors->>M1: commandOpen()
    Motors->>M2: commandOpen()
    State->>Controller: TransitionTo(MovingState(OPENING))
```

### Motor Interrupt Refresh Flow

```mermaid
sequenceDiagram
    participant HW as Hardware Registers
    participant System as AutomaticDoorSystem
    participant Motors as MotorPair
    participant M1 as MotorDriver(1)
    participant M2 as MotorDriver(2)
    participant Controller as DoorController

    HW-->>System: motor interrupt pending
    System->>Motors: handleInterrupts()
    Motors->>M1: handleOpenInterrupt()/handleClosedInterrupt()
    Motors->>M2: handleOpenInterrupt()/handleClosedInterrupt()
    Motors-->>System: interrupt handled
    System->>Controller: updateStateFromHardware()
    Controller->>Motors: getPosition()
    alt motors disagree or motion mismatches
        Controller->>Controller: TransitionTo(FaultState)
    else position == OPEN
        Controller->>Controller: TransitionTo(OpenState)
    else position == CLOSED
        Controller->>Controller: TransitionTo(ClosedState)
    else
        Controller->>Controller: TransitionTo(MovingState)
    end
```