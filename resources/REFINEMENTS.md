# Proposed Refinements

These items are not required to satisfy the current interview problem, but they are reasonable discussion points for future refinement.

1. Clarify dual-motor behavior.
   The customer memo lists two motors, but does not say whether one motor may continue alone or whether both positions must always match.

2. Define fault behavior and recovery path.
   The current requirements do not describe what the system should do after the controller enters `FaultState`. The current implementation ignores button input and provides no recovery path. A future refinement should define whether recovery requires an explicit operator reset, a long button hold, or a dedicated reset input. A concrete starting point would be adding a `recover()` method to `FaultState` that stops both motors, re-reads hardware, and transitions to the appropriate state if the fault condition is gone.

3. Clarify interrupt ownership and control flow expectations.
   The current design uses a small system-level coordinator to acknowledge hardware interrupts and then dispatch domain behavior. The customer memo does not say whether that flow should be polling-based, interrupt-driven, or event-loop based.

4. Define whether button release has any behavioral meaning.
   The current implementation acknowledges release interrupts but does not use them for door control because no requirement currently depends on button release.

5. Define acceptable command speed behavior.
   The current implementation uses a fixed default speed. A future revision could make speed configurable if required by product variants.

6. Add explicit requirements for safety and recovery.
   For a production-quality system, future requirements should define stop behavior, timeout behavior, retry policy, and degraded-mode handling. The current implementation provides none of these.

7. Extend fault detection to cover motor stall at non-endpoint positions.
   `hasFault()` currently checks for contradictory endpoint states and mismatched motion. It does not detect the case where both motors stop at non-endpoint positions, for example motor 1 at 1023 and motor 2 stalled at 900. In this scenario both report `isMoving() = false`, no fault is triggered, and `getPosition()` averages to an intermediate value. The controller enters `MovingState` indefinitely while the door is physically stuck. A future refinement should define a position-divergence threshold that triggers a fault when stopped motors disagree beyond an acceptable tolerance.

8. Replace dynamic state allocation with static pre-allocation.
   This is honestly a really big oversight in my design and I only realized it after doing further research into safety critical software systems. I initially followed [refactoring.guru's state pattern](<https://refactoring.guru/design-patterns/state/cpp/example#lang-features>) implementation for C++. In this pattern implementation each `TransitionTo` call currently uses `new` to construct a state object, which unintentionally introduces non-deterministic allocation time and heap fragmentation. This violates NASA's ["The Power of Ten"](<https://en.wikipedia.org/wiki/The_Power_of_10:_Rules_for_Developing_Safety-Critical_Code>) Rule #3: "Do not use dynamic memory allocation after initialization". A production implementation would pre-allocate all four state objects statically at startup and reuse them, or use placement-new into a fixed-size state pool. Either approach eliminates heap allocation while preserving the State pattern structure.

9. Harden `hasFault()` and `updateStateFromHardware()` against mid-read interrupts.
    Both functions perform multiple separate register reads that can be interleaved with hardware interrupts, producing an inconsistent snapshot. On real hardware, interrupts should be disabled for the duration of the read sequence, or hardware capture registers should be used if available. The current mock does not model concurrent register access, so this hazard is not exercised by the test suite.

10. FIXED! Replace `std::cout` diagnostics with a compile-time debug flag.
    The current implementation uses `std::cout` throughout state transitions, interrupt handlers, and motor commands. In a real embedded environment (as far as I know) stdout does not exist, and I/O in interrupt-adjacent paths introduces unbounded latency. All diagnostic output should be conditionally compiled using a debug flag and replaced with a platform-appropriate logging mechanism in production builds.