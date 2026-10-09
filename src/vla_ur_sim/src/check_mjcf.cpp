#include <chrono>
#include <iostream>
#include <thread>

#include <mujoco/mujoco.h>

int main(int argc, char **argv)
{
    if (argc != 2) mju_error("No MJCF model file provided (.xml)");

    std::string mjcf_path = argv[1];

    char error[1000] = "";
    mjModel* model = mj_loadXML(mjcf_path.c_str(), NULL, error, 1000);
    if (!model) mju_error("Failed to load model: %s", error);

    mjData* data = mj_makeData(model);
    if (!data) { mj_deleteModel(model); mju_error("Failed to allocate data"); }

    std::cout << "Model parsed successfully" << std::endl;

    mj_deleteData(data);
    mj_deleteModel(model);

    return 0;
}