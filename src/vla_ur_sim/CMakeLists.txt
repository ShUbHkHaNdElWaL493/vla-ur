cmake_minimum_required(VERSION 3.8)
project(vla_ur_sim)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

include_directories(include)

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(glfw3 REQUIRED)
find_package(mujoco REQUIRED)

add_executable(check_mjcf src/check_mjcf.cpp)
target_link_libraries(check_mjcf mujoco::mujoco)

add_executable(headless_mujoco_engine src/headless_mujoco_engine.cpp)
target_link_libraries(headless_mujoco_engine mujoco::mujoco)

add_executable(visual_mujoco_engine src/visual_mujoco_engine.cpp)
target_link_libraries(visual_mujoco_engine glfw mujoco::mujoco)

install(
  DIRECTORY include/
  DESTINATION include/
)

install(
  DIRECTORY launch models
  DESTINATION share/${PROJECT_NAME}
)

install(
  TARGETS check_mjcf headless_mujoco_engine visual_mujoco_engine
  DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
