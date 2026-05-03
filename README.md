# AutomaticDoorSystem

C++ implementation of an automatic door control system, developed as part of a software engineering interview assessment. The system models a 2-motor, single push-button door opener using a State design pattern, with memory-mapped I/O via a mock hardware layer.

## Build

Requires `clang++` with C++11 support.

Clone the repository:
```bash
git clone https://github.com/Jacob-Russell/AutomaticDoorSystem.git
cd AutomaticDoorSystem
```

Build and test using the provided Makefile:

| Command      | Description                              |
|--------------|------------------------------------------|
| `make`       | Compiles all source files                |
| `make test`  | Builds and runs the full test suite      |
| `make clean` | Removes all build artifacts              |

## Documentation

- GitHub Pages code documentation: <https://jacob-russell.github.io/AutomaticDoorSystem/>
- Architecture diagrams: [resources/ARCHITECTURE_DIAGRAMS.md](resources/ARCHITECTURE_DIAGRAMS.md)
- Requirements: [resources/REQUIREMENTS.md](resources/REQUIREMENTS.md)
- Assumptions: [resources/ASSUMPTIONS.md](resources/ASSUMPTIONS.md)
- Proposed refinements: [resources/REFINEMENTS.md](resources/REFINEMENTS.md)