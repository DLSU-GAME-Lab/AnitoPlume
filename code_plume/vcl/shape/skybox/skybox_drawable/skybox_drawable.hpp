#pragma once

#include "vcl/shape/skybox/skybox_structure/skybox.hpp"
#include "vcl/math/math.hpp"
#include "vcl/interaction/camera/camera.hpp"

#include "skybox_drawable_gpu_data/skybox_drawable_gpu_data.hpp"


namespace vcl
{
    /** High level OpenGL drawable structure.
     * skybox_drawable contains gpu data (skybox_drawable_gpu_data: VAO, VBO, etc.), and a set of uniform parameter (skybox_drawable_uniform)*/
    struct skybox_drawable
    {
    public:

        skybox_drawable();
        /** Initialize VAO and VBO from the mesh */
        skybox_drawable(const skybox& skybox_cpu, GLuint shader = 0, GLuint texture_id = 0);

        /** Clear buffers (VBO, VAO, etc) */
        void clear();

        /** Data attributes: VAO and VBO as well as the number of triangle */
        skybox_drawable_gpu_data data;
        GLuint shader;
        GLuint texture_id;
    };

    void draw(const skybox_drawable& drawable, const camera_scene& camera);
    void draw(const skybox_drawable& drawable, const camera_scene& camera, GLuint shader);
    void draw(const skybox_drawable& drawable, const camera_scene& camera, GLuint shader, GLuint texture_id);
}
