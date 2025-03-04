#pragma once


#include "vcl/wrapper/glad/glad.hpp"
#include "../../skybox_structure/skybox.hpp"


namespace vcl
{
    struct skybox_drawable_gpu_data
    {

        skybox_drawable_gpu_data();
        skybox_drawable_gpu_data(const skybox& skybox_cpu);

        /** Clear buffers */
        void clear();

        GLuint vao;

        GLuint vbo_vertices;   // (x,y,z) coordinates
    };

    /** Call raw OpenGL draw */
    void draw(const skybox_drawable_gpu_data& gpu_data);

}
