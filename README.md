# Autonomous Robot Engineering System: Talos

<p align="center">
  <img src="images/talos-final.png" alt="Talos Robot Rendering" width="500"/>
</p>

**Talos** (named after the [bronze automaton from Greek mythology](https://www.theoi.com/Gigante/GiganteTalos.html)) develops a two-wheel self-balancing robot, inspired by professional systems. The architecture prioritizes hierarchical control: deterministic stability via an _STM32 microcontroller_, complemented by higher-level planning and telemetry on a _Jetson Orin Nano_. This design aligns with industry standards for scalable autonomy, enabling data-driven iteration and refinement.

The project applies embedded systems, control theory, and observability tools to bridge software engineering with robotics hardware, following a phased approach to systematically manage complexity.

## High-Level Architecture
<p align="center">
  <img src="images/architecture.png" alt="Talos Architecture" width="850"/>
</p>

## Hardware Overview
- **Chassis and Actuation**: [Yahboom 2WD kit](https://category.yahboom.net/products/sbr-chassis-kit) with [JGB37-520 encoder motors](https://www.aslongdcmotor.com/photo/aslongdcmotor/document/26547/37mm%20Round%20Spur%20Gear%20Motor_PDF00.pdf) and [Sabertooth 2x12 driver](https://www.dimensionengineering.com/datasheets/Sabertooth2x12.pdf).
- **Real-Time Controller**: [STM32 NUCLEO-F446RE](https://www.st.com/en/microcontrollers-microprocessors/stm32f446re.html) for inner-loop control.
- **High-Level Compute**: [Jetson Orin Nano](https://developer.nvidia.com/embedded/learn/get-started-jetson-orin-nano-devkit) for planning, vision, and logging.
- **Sensors**: [BNO055 IMU](https://www.bosch-sensortec.com/media/boschsensortec/downloads/application_notes_1/bst-bno055-an007.pdf) for fused pitch and angular rate (onboard Kalman filter, I2C to STM32).
- **Power**: [3S LiPo battery](https://genstattu.com/bw/) with safety features.

<p align="center">
  <img src="images/bench-setup.jpg" alt="Phase 1 Bench Setup - wheels up configuration" width="500"/>
</p>

## Phase 1: Independent Wheel Velocity Control
With wheels elevated for safety, this phase integrates encoders and implements per-motor PI control for precise velocity tracking. It establishes a foundation for balance, addressing asymmetries such as stiction and deadband variability.

Telemetry flows from the STM32 via [UART](https://en.wikipedia.org/wiki/Universal_asynchronous_receiver-transmitter) to a Python bridge, visualized in [PlotJuggler](https://plotjuggler.io/) for analysis. The plots demonstrate velocity errors converging under step references, with moving RMS metrics illustrating control performance.

<p align="center">
  <img src="images/pid-plots.gif" alt="PI Velocity Control Telemetry" width="850"/>
</p>

Firmware is implemented in C using [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html). Custom modules handle encoder velocity estimation (quadrature timer-based ticks/sec computation, convertible to rad/s or m/s) and PI control (with integral limits, output clamping, and deadband compensation for stiction). The main loop initializes peripherals, executes PI steps at `~100 Hz`, applies PWM outputs, and logs metrics.

```mermaid
graph TD
    A[STM32 NUCLEO-F446RE] -->|PWM S1/S2| B[Sabertooth 2x12]
    B -->|Motor Outputs| C[Left/Right Motors]
    D[Encoders A/B] -->|Timer Inputs| A
    A -->|UART Telemetry| E[Python Bridge]
    E -->|UDP| F[PlotJuggler Visualization]
```

## Phase 2: IMU Integration and State Estimation

Phase 2 integrates the BNO055 IMU to produce the full robot state vector `[θ, θ̇, v]` — tilt angle, tilt rate, and wheel velocity. The BNO055 runs onboard sensor fusion (accel + gyro Kalman filter), outputting drift-free Euler pitch and low-latency Gyro X directly over I2C. Axis mapping and sign conventions were validated on the bench before chassis assembly.

Telemetry was extended to 11 signals and all IMU channels are live in PlotJuggler alongside the existing wheel velocity data.

## Roadmap
The project advances through structured phases to ensure robust development:

- **Phase 0** (Completed): Bench-top PWM sanity check for motor driver calibration and basic actuation verification.
- **Phase 1** (Completed): Encoder integration and independent wheel velocity control with PI loops.
- **Phase 2** (Completed): BNO055 IMU integration for state estimation (pitch θ, pitch rate θ̇) with validated sign conventions.
- **Phase 2.5** (In progress): Physical chassis assembly onto the Yahboom 2WD platform.
- **Phase 3**: STM32-based PID balance control.
- **Phase 4**: Jetson integration for setpoints, ROS2, and MCAP logging.
- **Phase 4.5**: Foxglove live visualization and telemetry routing via Jetson.
- **Phase 5**: Transition to LQR state feedback for enhanced stability.
- **Phase 6**: Outer-loop MPC on Jetson, incorporating vision.
- **Phase 7**: Extensions including disturbance rejection and learning-based controls.
