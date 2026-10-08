#include <chrono>
#include <iostream>
#include <thread>

#include <ament_index_cpp/get_package_share_directory.hpp>
#include <mujoco/mujoco.h>

int main()
{
    std::string mjcf_path = ament_index_cpp::get_package_share_directory("vla_ur_sim") + "/models/scene.xml";

    char error[1000] = "";
    mjModel* model = mj_loadXML(mjcf_path.c_str(), NULL, error, 1000);
    if (!model)
    {
        std::cerr << "Failed to load model: " << error << std::endl;
        return 1;
    }

    mjData* data = mj_makeData(model);
    if (!data)
    {
        std::cerr << "Failed to allocate data." << std::endl;
        mj_deleteModel(model);
        return 1;
    }

    mj_forward(model, data);

    size_t step = 0;
    while (true)
    {
        mj_step(model, data);
        std::cout << "Step: " << ++step << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    }

    mj_deleteData(data);
    mj_deleteModel(model);

    return 0;
}