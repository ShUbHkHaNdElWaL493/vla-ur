#pragma once

#include <filesystem>

#include <mujoco/mujoco.h>

class MuJoCoEngine
{
    private:

    mjVFS vfs;

    void populateVFS(const std::string& model_file)
    {
        std::filesystem::path model_path = model_file;
        std::filesystem::path parent_directory = model_path.parent_path();
        
        if (!std::filesystem::exists(parent_directory)) mju_error(
            "Parent directory does not exist: %s",
            parent_directory.string().c_str()
        );

        for (auto const& entry : std::filesystem::recursive_directory_iterator(parent_directory))
        {
            if (entry.is_regular_file())
            {
                if (entry.path() == model_path) continue;

                std::filesystem::path relative_path = std::filesystem::relative(entry.path(), parent_directory);

                std::string directory_string = parent_directory.string();
                std::string relative_string = relative_path.generic_string();

                int result = mj_addFileVFS(&vfs, directory_string.c_str(), relative_string.c_str());
                if (result != 0 && result != 2) mju_error(
                    "Failed to add asset to VFS: %s (error: %d)",
                    relative_string.c_str(),
                    result
                );
            }
        }
    }

    protected:

    mjModel* model;
    mjData* data;

    public:

    MuJoCoEngine(std::string model_file) : model(nullptr), data(nullptr)
    {
        mj_defaultVFS(&vfs);
        populateVFS(model_file);

        char error[1000] = "";
        model = mj_loadXML(model_file.c_str(), &vfs, error, 1000);
        if (!model) mju_error("Error in loading model: %s", error);

        // make data
        data = mj_makeData(model);
        if (!data)
        {
            mj_deleteModel(model);
            mju_error("Error in allocating data.");
        }
    }

    ~MuJoCoEngine()
    {
        // free MuJoCo model and data
        mj_deleteData(data);
        mj_deleteModel(model);
        mj_deleteVFS(&vfs);
    }

    virtual void loop() = 0;

};