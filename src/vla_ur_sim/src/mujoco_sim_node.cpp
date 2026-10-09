#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <GLFW/glfw3.h>
#include <mujoco/mujoco.h>

class MuJoCoSimNode
{

    private:

    mjModel* model;
    mjData* data;

    GLFWwindow* window;

    mjvCamera camera;
    mjvOption option;
    mjvScene scene;
    mjrContext context;

    bool button_left, button_middle, button_right;
    double lastx, lasty;

    // keyboard callback
    static void keyboard_callback(GLFWwindow* window, int key, int scancode, int act, int mods)
    {
        auto* instance = static_cast<MuJoCoSimNode*>(glfwGetWindowUserPointer(window));
        if (instance)
        {
            // backspace: reset simulation
            if (act == GLFW_PRESS && key == GLFW_KEY_BACKSPACE)
            {
                mj_resetData(instance->model, instance->data);
                mj_forward(instance->model, instance->data);
            }
        }
    }

    // mouse button callback
    static void mouse_button_callback(GLFWwindow* window, int button, int act, int mods)
    {
        auto* instance = static_cast<MuJoCoSimNode*>(glfwGetWindowUserPointer(window));
        if (instance)
        {
            // update button state
            instance->button_left = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
            instance->button_middle = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
            instance->button_right = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);

            // update mouse position
            glfwGetCursorPos(window, &(instance->lastx), &(instance->lasty));
        }
    }

    // mouse move callback
    static void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
    {
        auto* instance = static_cast<MuJoCoSimNode*>(glfwGetWindowUserPointer(window));
        if (instance)
        {
            // no buttons down: nothing to do
            if (!(instance->button_left) && !(instance->button_middle) && !(instance->button_right)) return;

            // compute mouse displacement, save
            double dx = xpos - instance->lastx;
            double dy = ypos - instance->lasty;
            instance->lastx = xpos;
            instance->lasty = ypos;

            // get current window size
            int width, height;
            glfwGetWindowSize(window, &width, &height);

            // get shift key state
            bool mod_shift = (
                glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
                glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS
            );

            // determine action based on mouse button
            mjtMouse action;
            if (instance->button_right) { action = mod_shift ? mjMOUSE_MOVE_H : mjMOUSE_MOVE_V; }
            else if (instance->button_left) { action = mod_shift ? mjMOUSE_ROTATE_H : mjMOUSE_ROTATE_V; }
            else { action = mjMOUSE_ZOOM; }

            // move camera
            mjv_moveCamera(instance->model, action, dx / height, dy / height, &(instance->camera));
        }
    }

    // scroll callback
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        auto* instance = static_cast<MuJoCoSimNode*>(glfwGetWindowUserPointer(window));
        if (instance)
        {
            // emulate vertical mouse motion = 5% of window height
            mjv_moveCamera(instance->model, mjMOUSE_ZOOM, 0, -0.05 * yoffset, &(instance->camera));
        }
    }

    public:

    MuJoCoSimNode(std::string model_file) :
    model(NULL), data(NULL),
    button_left(false), button_middle(false), button_right(false),
    lastx(0), lasty(0)
    {
        char error[1000] = "";
        model = mj_loadXML(model_file.c_str(), 0, error, 1000);
        if (!model) mju_error("Error in loading model: %s", error);

        // make data
        data = mj_makeData(model);
        if (!data)
        {
            mj_deleteModel(model);
            mju_error("Error in allocating data.");
        }

        // init GLFW
        if (!glfwInit()) mju_error("Error in initializing GLFW.");

        // create window, make OpenGL context current, request v-sync
        window = glfwCreateWindow(1200, 900, "MuJoCo", NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            mju_error("Failed to create GLFW window.");
        }
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);
        glfwSetWindowUserPointer(window, this);

        // initialize visualization data structures
        mjv_defaultCamera(&camera);
        mjv_defaultOption(&option);
        mjv_defaultScene(&scene);
        mjr_defaultContext(&context);

        // create scene and context
        mjv_makeScene(model, &scene, 2000);
        mjr_makeContext(model, &context, mjFONTSCALE_150);

        // install GLFW mouse and keyboard callbacks
        glfwSetKeyCallback(window, keyboard_callback);
        glfwSetCursorPosCallback(window, mouse_move_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetScrollCallback(window, scroll_callback);
    }

    ~MuJoCoSimNode()
    {
        // free visualization storage
        mjv_freeScene(&scene);
        mjr_freeContext(&context);

        // free MuJoCo model and data
        mj_deleteData(data);
        mj_deleteModel(model);

        // terminate GLFW (crashes with Linux NVidia drivers)
        #if defined(__APPLE__) || defined(_WIN32)
        glfwTerminate();
        #endif
    }

    void render()
    {
        while (!glfwWindowShouldClose(window))
        {
            // advance interactive simulation for 1/60 sec
            // Assuming MuJoCo can simulate faster than real-time, which it usually can,
            // this loop will finish on time for the next frame to be rendered at 60 fps.
            // Otherwise add a cpu timer and exit this loop when it is time to render.
            mjtNum simstart = data->time;
            while (data->time - simstart < 1.0 / 60.0) mj_step(model, data);

            // get framebuffer viewport
            mjrRect viewport = {0, 0, 0, 0};
            glfwGetFramebufferSize(window, &viewport.width, &viewport.height);

            // update scene and render
            mjv_updateScene(model, data, &option, NULL, &camera, mjCAT_ALL, &scene);
            mjr_render(viewport, &scene, &context);

            // swap OpenGL buffers (blocking call due to v-sync)
            glfwSwapBuffers(window);

            // process pending GUI events, call GLFW callbacks
            glfwPollEvents();
        }
    }

};

int main(int argc, char** argv)
{
    std::vector<std::string> args(argv, argv + argc);

    if (argc == 2)
    {
        MuJoCoSimNode node(args[1]);
        node.render();
        return 0;
    }

    if (argc > 2 && args[2] == "--ros-args")
    {
        MuJoCoSimNode node(args[1]);
        node.render();
        return 0;
    } else mju_error("Provide a model file path as an argument for the simulator.");
}