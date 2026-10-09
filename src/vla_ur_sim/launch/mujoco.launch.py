from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, ExecuteProcess
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():

    model = LaunchConfiguration('model')
    
    mujoco_sim_node = Node(
        package='vla_ur_sim',
        executable='mujoco_sim_node',
        output='screen',
        arguments=[model]
    )

    return LaunchDescription([
        DeclareLaunchArgument(
            'model',
            description='MJCF Model file to load in MuJoCo.',
            default_value=PathJoinSubstitution([
                FindPackageShare('vla_ur_sim'),
                'models',
                'scene.xml'
            ])
        ),
        mujoco_sim_node
    ])
