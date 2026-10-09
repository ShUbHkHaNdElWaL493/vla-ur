# Simulation

This document outlines the containerized installation and configuration of the physics simulation layer required to evaluate the Vision-Language-Action (VLA) model. We utilize **MuJoCo** as the primary simulation framework.

To ensure reproducibility, isolate dependencies, and support future integration via decoupled processes (e.g., ROS 2), the simulation runs inside a dedicated Ubuntu 24.04 Docker container.

## System Requirements

*   **OS:** Linux (Ubuntu 20.04+ recommended) or WSL2 on Windows.
*   **Docker:** Docker Engine and Docker Compose V2.
*   **GPU (Optional but Recommended):** NVIDIA GPU with the [NVIDIA Container Toolkit](https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/latest/install-guide.html) installed for hardware-accelerated camera rendering.

## Docker Setup and Installation

We manage the decoupled architecture using Docker Compose profiles. You can launch the simulation container with or without GPU acceleration depending on your hardware.

1.  Clone the project directory.
2.  Build and launch the simulation container using the profile that matches your hardware setup.
    ```bash
    # For Systems with an NVIDIA GPU
    docker compose --profile gpu up -d vla-ur-sim-gpu

    # For Systems without a GPU
    docker compose --profile cpu up -d vla-ur-sim-cpu
    ```

## Verification

To verify that MuJoCo is correctly installed and rendering within the container, you can execute a test script interactively.

1. Open a bash shell inside your running container.
    ```bash
    # If using the GPU profile
    docker exec -it vla-ur-sim-gpu bash

    # If using the CPU profile
    docker exec -it vla-ur-sim-cpu bash
    ```

2. Inside the container, launch the `mujoco_sim_node`, present inside the `vla_ur_sim` ROS2 package.
    ```bash
    ros2 launch vla_ur_sim mujoco.launch.py
    ```

3. If you see a GLFW window with a UR5e, your MuJoCo environment is working.