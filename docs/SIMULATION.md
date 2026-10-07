# Simulation

This document outlines the containerized installation and configuration of the physics simulation layer required to evaluate the Vision-Language-Action (VLA) model. We utilize **Robosuite** (powered by MuJoCo) as the primary simulation framework.

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
    # For Systems with an NVIDIA GPU (Hardware-Accelerated Rendering)
    docker compose --profile gpu up -d vla-ur-sim-gpu

    # For Systems without a GPU (CPU-Only / OSMesa Rendering)
    docker compose --profile cpu up -d vla-ur-sim-cpu
    ```

## Environment Configuration

To interface with a VLA, the simulation must be initialized with an **OSC Pose Controller**. This allows the environment to accept action vectors structured as `[dx, dy, dz, droll, dpitch, dyaw, gripper_state]`.

### Controller Configuration (`osc_pose.json`)
You can mount this file into the `/vla_ur` directory or define it directly in your python scripts.

```json
{
    "type": "OSC_POSE",
    "input_max": 1,
    "input_min": -1,
    "output_max": [0.05, 0.05, 0.05, 0.5, 0.5, 0.5],
    "output_min": [-0.05, -0.05, -0.05, -0.5, -0.5, -0.5],
    "kp": 150,
    "damping_ratio": 1,
    "impedance_mode": "fixed",
    "kp_limits": [0, 300],
    "damping_ratio_limits": [0, 10],
    "uncouple_pos_ori": true,
    "control_delta": true
}
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

2. Inside the container, create a quick python script (`test_sim.py`) that initializes a headless environment and captures a frame.
    ```python
    import robosuite as suite
    import numpy as np

    # Initialize a headless environment for off-screen rendering validation
    env = suite.make(
        env_name="Lift",
        robots="Panda",
        has_renderer=False,          
        has_offscreen_renderer=True, 
        use_camera_obs=True,
        camera_names="agentview",    
        control_freq=20,
    )

    obs = env.reset()
    print("Simulation initialized successfully.")
    print("Camera Observation Shape:", obs['agentview_image'].shape)
    ```

3. Run it via `python test_sim.py`. If it successfully prints the array shape (e.g., `(256, 256, 3)`), your headless MuJoCo environment is ready to send images to the VLA container.