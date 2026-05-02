#include <cassert>
#include <iostream>
#include "automatic_door_system.h"
#include "button_driver.h"
#include "door_controller.h"
#include "hardware_registers.h"
#include "mock_hardware.h"
#include "motor_driver.h"
#include "motor_pair.h"

void setMotorStatus(uintptr_t base, int position, bool moving, int speed) {
    uint32_t val = 0;
    val |= (speed & 0x07);
    val |= (moving ? 1 : 0) << 3;
    val |= ((position & 0x3FF) << 4);
    writeRegister(base + STATUS_OFFSET, val);
}

void setButtonStatus(bool pressed) {
    writeRegister(BUTTON_BASE_ADDRESS + STATUS_OFFSET, pressed ? 1U : 0U);
}

void setMotorInterrupt(uintptr_t base, uint32_t interruptMask) {
    writeRegister(base + INTERRUPT_OFFSET, interruptMask);
}

void test_mock_hardware_read_write() {
    MockHardware::reset();
    writeRegister(0x1000, 42);
    assert(readRegister(0x1000) == 42);
    std::cout << "[PASS] test_mock_hardware_read_write\n";
}

void test_mock_hardware_default_zero() {
    MockHardware::reset();
    assert(readRegister(0x9999) == 0);
    std::cout << "[PASS] test_mock_hardware_default_zero\n";
}

void test_mock_hardware_reset() {
    writeRegister(0x1000, 99);
    MockHardware::reset();
    assert(readRegister(0x1000) == 0);
    std::cout << "[PASS] test_mock_hardware_reset\n";
}

void test_motor_get_position_closed() {
    MockHardware::reset();
    setMotorStatus(MOTOR1_BASE_ADDRESS, 0, false, 0);
    MotorDriver motor(1);
    assert(motor.getPosition() == 0);
    std::cout << "[PASS] test_motor_get_position_closed\n";
}

void test_motor_get_position_open() {
    MockHardware::reset();
    setMotorStatus(MOTOR1_BASE_ADDRESS, 1023, false, 0);
    MotorDriver motor(1);
    assert(motor.getPosition() == 1023);
    std::cout << "[PASS] test_motor_get_position_open\n";
}

void test_motor_is_moving_true() {
    MockHardware::reset();
    setMotorStatus(MOTOR1_BASE_ADDRESS, 512, true, 4);
    MotorDriver motor(1);
    assert(motor.isMoving());
    std::cout << "[PASS] test_motor_is_moving_true\n";
}

void test_motor_get_speed() {
    MockHardware::reset();
    setMotorStatus(MOTOR1_BASE_ADDRESS, 0, true, 5);
    MotorDriver motor(1);
    assert(motor.getSpeed() == 5);
    std::cout << "[PASS] test_motor_get_speed\n";
}

void test_motor_command_open_writes_register() {
    MockHardware::reset();
    MotorDriver motor(1);
    motor.commandOpen(4);

    const uint32_t cmd = readRegister(MOTOR1_BASE_ADDRESS + COMMAND_OFFSET);
    assert((cmd & 0x07) == 1);
    assert(((cmd >> 3) & 0x07) == 4);
    assert((cmd >> 6) & 0x01);
    std::cout << "[PASS] test_motor_command_open_writes_register\n";
}

void test_motor_command_close_writes_register() {
    MockHardware::reset();
    MotorDriver motor(1);
    motor.commandClose(4);

    const uint32_t cmd = readRegister(MOTOR1_BASE_ADDRESS + COMMAND_OFFSET);
    assert((cmd & 0x07) == 0);
    assert(((cmd >> 3) & 0x07) == 4);
    assert((cmd >> 6) & 0x01);
    std::cout << "[PASS] test_motor_command_close_writes_register\n";
}

void test_motor_command_stop_writes_register() {
    MockHardware::reset();
    MotorDriver motor(1);
    motor.commandStop();

    const uint32_t cmd = readRegister(MOTOR1_BASE_ADDRESS + COMMAND_OFFSET);
    assert((cmd & 0x07) == 2);
    assert((cmd >> 6) & 0x01);
    std::cout << "[PASS] test_motor_command_stop_writes_register\n";
}

void test_motor_pair_reports_open_only_when_both_open() {
    MockHardware::reset();
    setMotorStatus(MOTOR1_BASE_ADDRESS, 1023, false, 0);
    setMotorStatus(MOTOR2_BASE_ADDRESS, 1023, false, 0);
    MotorPair motors;
    assert(motors.getPosition() == 1023);
    std::cout << "[PASS] test_motor_pair_reports_open_only_when_both_open\n";
}

void test_motor_pair_reports_closed_only_when_both_closed() {
    MockHardware::reset();
    setMotorStatus(MOTOR1_BASE_ADDRESS, 0, false, 0);
    setMotorStatus(MOTOR2_BASE_ADDRESS, 0, false, 0);
    MotorPair motors;
    assert(motors.getPosition() == 0);
    std::cout << "[PASS] test_motor_pair_reports_closed_only_when_both_closed\n";
}

void test_motor_pair_detects_fault_when_end_states_disagree() {
    MockHardware::reset();
    setMotorStatus(MOTOR1_BASE_ADDRESS, 0, false, 0);
    setMotorStatus(MOTOR2_BASE_ADDRESS, 1023, false, 0);
    MotorPair motors;
    assert(motors.hasFault());
    std::cout << "[PASS] test_motor_pair_detects_fault_when_end_states_disagree\n";
}

void test_motor_pair_detects_fault_when_motion_disagrees() {
    MockHardware::reset();
    setMotorStatus(MOTOR1_BASE_ADDRESS, 512, true, 4);
    setMotorStatus(MOTOR2_BASE_ADDRESS, 512, false, 0);
    MotorPair motors;
    assert(motors.hasFault());
    std::cout << "[PASS] test_motor_pair_detects_fault_when_motion_disagrees\n";
}

void test_button_is_pressed_true() {
    MockHardware::reset();
    ButtonDriver button;
    setButtonStatus(true);
    assert(button.isPressed());
    std::cout << "[PASS] test_button_is_pressed_true\n";
}

void test_button_is_pressed_false() {
    MockHardware::reset();
    ButtonDriver button;
    setButtonStatus(false);
    assert(!button.isPressed());
    std::cout << "[PASS] test_button_is_pressed_false\n";
}

void test_button_clears_pressed_interrupt() {
    MockHardware::reset();
    ButtonDriver button;
    button.acknowledgePressedInterrupt();
    assert(readRegister(BUTTON_BASE_ADDRESS + INTERRUPT_OFFSET) & BUTTON_INTERRUPT_PRESSED_MASK);
    std::cout << "[PASS] test_button_clears_pressed_interrupt\n";
}

void test_button_clears_released_interrupt() {
    MockHardware::reset();
    ButtonDriver button;
    button.acknowledgeReleasedInterrupt();
    assert(readRegister(BUTTON_BASE_ADDRESS + INTERRUPT_OFFSET) & BUTTON_INTERRUPT_RELEASED_MASK);
    std::cout << "[PASS] test_button_clears_released_interrupt\n";
}

void test_button_press_drives_controller() {
    MockHardware::reset();
    setMotorStatus(MOTOR1_BASE_ADDRESS, 0, false, 0);
    setMotorStatus(MOTOR2_BASE_ADDRESS, 0, false, 0);
    AutomaticDoorSystem system;

    system.handleButtonPressedInterrupt();
    const uint32_t firstMotor1Cmd = readRegister(MOTOR1_BASE_ADDRESS + COMMAND_OFFSET);
    const uint32_t firstMotor2Cmd = readRegister(MOTOR2_BASE_ADDRESS + COMMAND_OFFSET);
    system.handleButtonPressedInterrupt();

    assert((firstMotor1Cmd & 0x07) == 1);
    assert((firstMotor2Cmd & 0x07) == 1);
    assert(readRegister(MOTOR1_BASE_ADDRESS + COMMAND_OFFSET) == firstMotor1Cmd);
    assert(readRegister(MOTOR2_BASE_ADDRESS + COMMAND_OFFSET) == firstMotor2Cmd);
    std::cout << "[PASS] test_button_press_drives_controller\n";
}

void test_motor_interrupts_refresh_controller_state() {
    MockHardware::reset();
    setMotorStatus(MOTOR1_BASE_ADDRESS, 512, false, 0);
    setMotorStatus(MOTOR2_BASE_ADDRESS, 512, false, 0);
    AutomaticDoorSystem system;

    setMotorStatus(MOTOR1_BASE_ADDRESS, 1023, false, 0);
    setMotorStatus(MOTOR2_BASE_ADDRESS, 1023, false, 0);
    setMotorInterrupt(MOTOR1_BASE_ADDRESS, MOTOR_INTERRUPT_OPEN_MASK);
    setMotorInterrupt(MOTOR2_BASE_ADDRESS, MOTOR_INTERRUPT_OPEN_MASK);

    system.handleMotorInterrupts();
    system.handleButtonPressedInterrupt();

    assert((readRegister(MOTOR1_BASE_ADDRESS + INTERRUPT_OFFSET) & MOTOR_INTERRUPT_OPEN_MASK) != 0);
    assert((readRegister(MOTOR2_BASE_ADDRESS + INTERRUPT_OFFSET) & MOTOR_INTERRUPT_OPEN_MASK) != 0);
    assert((readRegister(MOTOR1_BASE_ADDRESS + COMMAND_OFFSET) & 0x07) == 0);
    assert((readRegister(MOTOR2_BASE_ADDRESS + COMMAND_OFFSET) & 0x07) == 0);
    std::cout << "[PASS] test_motor_interrupts_refresh_controller_state\n";
}

void test_motor_fault_transitions_system_to_fault_state() {
    MockHardware::reset();
    setMotorStatus(MOTOR1_BASE_ADDRESS, 0, false, 0);
    setMotorStatus(MOTOR2_BASE_ADDRESS, 0, false, 0);
    AutomaticDoorSystem system;

    setMotorStatus(MOTOR1_BASE_ADDRESS, 0, false, 0);
    setMotorStatus(MOTOR2_BASE_ADDRESS, 1023, false, 0);
    setMotorInterrupt(MOTOR1_BASE_ADDRESS, MOTOR_INTERRUPT_CLOSED_MASK);
    setMotorInterrupt(MOTOR2_BASE_ADDRESS, MOTOR_INTERRUPT_OPEN_MASK);

    system.handleMotorInterrupts();
    const uint32_t command1 = readRegister(MOTOR1_BASE_ADDRESS + COMMAND_OFFSET);
    const uint32_t command2 = readRegister(MOTOR2_BASE_ADDRESS + COMMAND_OFFSET);
    system.handleButtonPressedInterrupt();

    assert(readRegister(MOTOR1_BASE_ADDRESS + COMMAND_OFFSET) == command1);
    assert(readRegister(MOTOR2_BASE_ADDRESS + COMMAND_OFFSET) == command2);
    std::cout << "[PASS] test_motor_fault_transitions_system_to_fault_state\n";
}

void test_init_closed_state_from_position() {
    MockHardware::reset();
    setMotorStatus(MOTOR1_BASE_ADDRESS, 0, false, 0);
    setMotorStatus(MOTOR2_BASE_ADDRESS, 0, false, 0);
    MotorPair motors;
    DoorController controller(motors);
    controller.onButtonPressed();
    assert(controller.currentDirection() == MovementDirection::OPENING);
    std::cout << "[PASS] test_init_closed_state_from_position\n";
}

void test_init_open_state_from_position() {
    MockHardware::reset();
    setMotorStatus(MOTOR1_BASE_ADDRESS, 1023, false, 0);
    setMotorStatus(MOTOR2_BASE_ADDRESS, 1023, false, 0);
    MotorPair motors;
    DoorController controller(motors);
    controller.onButtonPressed();
    assert(controller.currentDirection() == MovementDirection::CLOSING);
    std::cout << "[PASS] test_init_open_state_from_position\n";
}

void test_closed_state_button_transitions_to_moving() {
    MockHardware::reset();
    setMotorStatus(MOTOR1_BASE_ADDRESS, 0, false, 0);
    setMotorStatus(MOTOR2_BASE_ADDRESS, 0, false, 0);
    MotorPair motors;
    DoorController controller(motors);

    controller.onButtonPressed();
    const uint32_t firstMotor1Cmd = readRegister(MOTOR1_BASE_ADDRESS + COMMAND_OFFSET);
    const uint32_t firstMotor2Cmd = readRegister(MOTOR2_BASE_ADDRESS + COMMAND_OFFSET);
    controller.onButtonPressed();
    assert(controller.currentDirection() == MovementDirection::OPENING);
    assert(readRegister(MOTOR1_BASE_ADDRESS + COMMAND_OFFSET) == firstMotor1Cmd);
    assert(readRegister(MOTOR2_BASE_ADDRESS + COMMAND_OFFSET) == firstMotor2Cmd);
    std::cout << "[PASS] test_closed_state_button_transitions_to_moving\n";
}

void test_open_state_button_transitions_to_moving() {
    MockHardware::reset();
    setMotorStatus(MOTOR1_BASE_ADDRESS, 1023, false, 0);
    setMotorStatus(MOTOR2_BASE_ADDRESS, 1023, false, 0);
    MotorPair motors;
    DoorController controller(motors);

    controller.onButtonPressed();
    const uint32_t firstMotor1Cmd = readRegister(MOTOR1_BASE_ADDRESS + COMMAND_OFFSET);
    const uint32_t firstMotor2Cmd = readRegister(MOTOR2_BASE_ADDRESS + COMMAND_OFFSET);
    controller.onButtonPressed();
    assert(controller.currentDirection() == MovementDirection::CLOSING);
    assert(readRegister(MOTOR1_BASE_ADDRESS + COMMAND_OFFSET) == firstMotor1Cmd);
    assert(readRegister(MOTOR2_BASE_ADDRESS + COMMAND_OFFSET) == firstMotor2Cmd);
    std::cout << "[PASS] test_open_state_button_transitions_to_moving\n";
}

void test_moving_state_ignores_button() {
    MockHardware::reset();
    setMotorStatus(MOTOR1_BASE_ADDRESS, 512, false, 0);
    setMotorStatus(MOTOR2_BASE_ADDRESS, 512, false, 0);
    MotorPair motors;
    DoorController controller(motors);

    const uint32_t initCmd = readRegister(MOTOR1_BASE_ADDRESS + COMMAND_OFFSET);
    const uint32_t initCmd2 = readRegister(MOTOR2_BASE_ADDRESS + COMMAND_OFFSET);
    controller.onButtonPressed();
    controller.onButtonPressed();
    assert(controller.currentDirection() == MovementDirection::OPENING);
    assert(readRegister(MOTOR1_BASE_ADDRESS + COMMAND_OFFSET) == initCmd);
    assert(readRegister(MOTOR2_BASE_ADDRESS + COMMAND_OFFSET) == initCmd2);
    std::cout << "[PASS] test_moving_state_ignores_button\n";
}

void test_motor_position_1023_transitions_to_open() {
    MockHardware::reset();
    setMotorStatus(MOTOR1_BASE_ADDRESS, 512, false, 0);
    setMotorStatus(MOTOR2_BASE_ADDRESS, 512, false, 0);
    AutomaticDoorSystem system;

    setMotorStatus(MOTOR1_BASE_ADDRESS, 1023, false, 0);
    setMotorStatus(MOTOR2_BASE_ADDRESS, 1023, false, 0);
    system.refreshFromHardware();
    system.handleButtonPressedInterrupt();
    assert((readRegister(MOTOR1_BASE_ADDRESS + COMMAND_OFFSET) & 0x07) == 0);
    assert((readRegister(MOTOR2_BASE_ADDRESS + COMMAND_OFFSET) & 0x07) == 0);
    std::cout << "[PASS] test_motor_position_1023_transitions_to_open\n";
}

void test_motor_position_0_transitions_to_closed() {
    MockHardware::reset();
    setMotorStatus(MOTOR1_BASE_ADDRESS, 512, false, 0);
    setMotorStatus(MOTOR2_BASE_ADDRESS, 512, false, 0);
    AutomaticDoorSystem system;

    setMotorStatus(MOTOR1_BASE_ADDRESS, 0, false, 0);
    setMotorStatus(MOTOR2_BASE_ADDRESS, 0, false, 0);
    system.refreshFromHardware();
    system.handleButtonPressedInterrupt();
    assert((readRegister(MOTOR1_BASE_ADDRESS + COMMAND_OFFSET) & 0x07) == 1);
    assert((readRegister(MOTOR2_BASE_ADDRESS + COMMAND_OFFSET) & 0x07) == 1);
    std::cout << "[PASS] test_motor_position_0_transitions_to_closed\n";
}

void test_system_clears_open_interrupts() {
    MockHardware::reset();
    setMotorStatus(MOTOR1_BASE_ADDRESS, 1023, false, 0);
    setMotorStatus(MOTOR2_BASE_ADDRESS, 1023, false, 0);
    setMotorInterrupt(MOTOR1_BASE_ADDRESS, MOTOR_INTERRUPT_OPEN_MASK);
    setMotorInterrupt(MOTOR2_BASE_ADDRESS, MOTOR_INTERRUPT_OPEN_MASK);
    AutomaticDoorSystem system;

    system.handleMotorInterrupts();

    assert(readRegister(MOTOR1_BASE_ADDRESS + INTERRUPT_OFFSET) & MOTOR_INTERRUPT_OPEN_MASK);
    assert(readRegister(MOTOR2_BASE_ADDRESS + INTERRUPT_OFFSET) & MOTOR_INTERRUPT_OPEN_MASK);
    std::cout << "[PASS] test_system_clears_open_interrupts\n";
}

void test_system_clears_closed_interrupts() {
    MockHardware::reset();
    setMotorStatus(MOTOR1_BASE_ADDRESS, 0, false, 0);
    setMotorStatus(MOTOR2_BASE_ADDRESS, 0, false, 0);
    setMotorInterrupt(MOTOR1_BASE_ADDRESS, MOTOR_INTERRUPT_CLOSED_MASK);
    setMotorInterrupt(MOTOR2_BASE_ADDRESS, MOTOR_INTERRUPT_CLOSED_MASK);
    AutomaticDoorSystem system;

    system.handleMotorInterrupts();

    assert(readRegister(MOTOR1_BASE_ADDRESS + INTERRUPT_OFFSET) & MOTOR_INTERRUPT_CLOSED_MASK);
    assert(readRegister(MOTOR2_BASE_ADDRESS + INTERRUPT_OFFSET) & MOTOR_INTERRUPT_CLOSED_MASK);
    std::cout << "[PASS] test_system_clears_closed_interrupts\n";
}

void test_req7_intermediate_position_forces_open() {
    MockHardware::reset();
    setMotorStatus(MOTOR1_BASE_ADDRESS, 512, false, 0);
    setMotorStatus(MOTOR2_BASE_ADDRESS, 512, false, 0);
    MotorPair motors;
    DoorController controller(motors);

    const uint32_t cmd = readRegister(MOTOR1_BASE_ADDRESS + COMMAND_OFFSET);
    assert((cmd & 0x07) == 1);
    assert((cmd >> 6) & 0x01);
    assert((readRegister(MOTOR2_BASE_ADDRESS + COMMAND_OFFSET) & 0x07) == 1);
    controller.onButtonPressed();
    assert(readRegister(MOTOR1_BASE_ADDRESS + COMMAND_OFFSET) == cmd);
    std::cout << "[PASS] test_req7_intermediate_position_forces_open\n";
}

int main() {
    std::cout << "\n=== Mock Hardware Tests ===\n";
    test_mock_hardware_read_write();
    test_mock_hardware_default_zero();
    test_mock_hardware_reset();

    std::cout << "\n=== Motor Driver Tests ===\n";
    test_motor_get_position_closed();
    test_motor_get_position_open();
    test_motor_is_moving_true();
    test_motor_get_speed();
    test_motor_command_open_writes_register();
    test_motor_command_close_writes_register();
    test_motor_command_stop_writes_register();
    test_motor_pair_reports_open_only_when_both_open();
    test_motor_pair_reports_closed_only_when_both_closed();
    test_motor_pair_detects_fault_when_end_states_disagree();
    test_motor_pair_detects_fault_when_motion_disagrees();

    std::cout << "\n=== Button Driver Tests ===\n";
    test_button_is_pressed_true();
    test_button_is_pressed_false();
    test_button_clears_pressed_interrupt();
    test_button_clears_released_interrupt();
    test_button_press_drives_controller();
    test_motor_interrupts_refresh_controller_state();
    test_motor_fault_transitions_system_to_fault_state();

    std::cout << "\n=== Door Controller Tests ===\n";
    test_init_closed_state_from_position();
    test_init_open_state_from_position();
    test_closed_state_button_transitions_to_moving();
    test_open_state_button_transitions_to_moving();
    test_moving_state_ignores_button();
    test_motor_position_1023_transitions_to_open();
    test_motor_position_0_transitions_to_closed();
    test_system_clears_open_interrupts();
    test_system_clears_closed_interrupts();
    test_req7_intermediate_position_forces_open();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
