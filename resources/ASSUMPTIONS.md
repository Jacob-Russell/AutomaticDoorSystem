# Assumptions

1. The two motors act as a single door actuator.
   The current design uses `MotorPair` to command both motors together for open, close, and stop. This means that the current implementation assumes the two motors represent one physical actuator pair and should report matching motion and end states.

2. Door position is derived from the paired motor feedback.
   The two motors are assumed to remain synchronized during normal operation. If they report contradictory end states or mismatched motion, the current implementation transitions the controller into a fault state. A motor stall at a non-endpoint position is not currently detected as a fault; this is a known gap described in Proposed Refinements.

3. A button pressed interrupt represents a user request to toggle door motion.
   The system acknowledges the button interrupt and then forwards that event to the door controller.

4. Intermediate door position is treated as `MOVING` for control purposes.
   Any position other than fully closed (`0`) or fully open (`1023`) is handled as an in-motion or intermediate state.

5. On startup, an intermediate position is treated as a safe recovery condition that should command the door open.
   This is based directly on system requirement 7.

6. Speed selection is fixed at the current default value unless future requirements say otherwise.
   The software currently uses the motor driver's default open/close speed.

7. Register access is simulated in the development environment.
   `MockHardware` stands in for memory-mapped I/O during testing. On real hardware, all register reads and writes would use `volatile`-qualified pointers to prevent the compiler from caching register values. The mock does not enforce this, meaning compiler optimization hazards present on real hardware are not exercised by the current test suite. Additionally, the mock does not model concurrent register access, so race conditions between interrupt handlers and the main control loop are not caught by the current tests.

8. Interrupt acknowledgment assumes write-1-to-clear (W1C) semantics.
   `clearInterruptFlag` reads the interrupt register and writes back the flag mask using `|=`, which sets the target bit to 1. This is correct if the hardware clears the interrupt when firmware writes a 1 to that bit, which is the most common convention in embedded peripherals. The mock does not enforce either convention, so both W1C and write-0-to-clear implementations would pass the current tests. On real hardware this assumption must be verified against the device datasheet before use.

9. Consecutive register reads within a single operation are assumed to be uninterrupted.
   `hasFault()` and `updateStateFromHardware()` perform multiple separate register reads to derive a consistent hardware snapshot. The current implementation does not disable interrupts around these reads. On real hardware an interrupt could fire between reads and change motor state mid-evaluation, producing an inconsistent snapshot. In a production system, interrupts would be disabled for the duration of the snapshot or hardware capture registers would be used.

10. Fault handling is out of scope for the current interview implementation.
    The current solution detects basic motor disagreement and transitions to `FaultState`, but it does not implement explicit fault recovery or operator reset behavior.

11. Dynamic memory allocation is used for state object construction in this implementation.
    Each `TransitionTo` call allocates a new concrete state object using `new`. This approach was chosen to demonstrate the State pattern clearly. See refinement #8: [resources/REFINEMENTS.md](REFINEMENTS.md).