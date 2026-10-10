from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition, UnlessCondition
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():

    headless = LaunchConfiguration('headless')
    model = LaunchConfiguration('model')

    headless_mujoco_engine_node = Node(
        package='vla_ur_sim',
        executable='headless_mujoco_engine',
        condition=IfCondition(headless),
        output='screen',
        arguments=[model]
    )
    
    visual_mujoco_engine_node = Node(
        package='vla_ur_sim',
        executable='visual_mujoco_engine',
        condition=UnlessCondition(headless),
        output='screen',
        arguments=[model]
    )

    return LaunchDescription([
        DeclareLaunchArgument(
            'headless',
            description='Run the MuJoCo engine headless.',
            default_value='false'
        ),
        DeclareLaunchArgument(
            'model',
            description='MJCF Model file to load in MuJoCo.',
            default_value=PathJoinSubstitution([
                FindPackageShare('vla_ur_sim'),
                'models',
                'scene.xml'
            ])
        ),
        headless_mujoco_engine_node,
        visual_mujoco_engine_node
    ])
