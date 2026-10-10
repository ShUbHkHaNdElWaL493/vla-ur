#include "vla_ur_sim/mujoco_engine.hpp"

class HeadlessMuJoCoEngine : public MuJoCoEngine
{
    public:

        HeadlessMuJoCoEngine(std::string model_file) : MuJoCoEngine(model_file)
        {}

        void loop() override
        {
            while (true)
            {
                // advance interactive simulation for 1/60 sec
                // Assuming MuJoCo can simulate faster than real-time, which it usually can,
                // this loop will finish on time for the next frame to be rendered at 60 fps.
                // Otherwise add a cpu timer and exit this loop when it is time to render.
                mjtNum simstart = data->time;
                while (data->time - simstart < 1.0 / 60.0) mj_step(model, data);
            }
        }
};

int main(int argc, char** argv)
{
    std::vector<std::string> args(argv, argv + argc);

    if (argc == 2)
    {
        HeadlessMuJoCoEngine node(args[1]);
        node.loop();
        return 0;
    }

    if (argc > 2 && args[2] == "--ros-args")
    {
        HeadlessMuJoCoEngine node(args[1]);
        node.loop();
        return 0;
    } else mju_error("Check executable arguments.");
}