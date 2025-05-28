#include "skybox_drawable.hpp"

#include "vcl/opengl/opengl.hpp"

namespace vcl
{
    skybox_drawable::skybox_drawable()
        :data(), shader(0), texture_id(0)
    {}

    skybox_drawable::skybox_drawable(const skybox& skybox_arg, GLuint shader_arg, GLuint texture_id_arg)
        :data(skybox_arg), shader(shader_arg), texture_id(texture_id_arg)
    {}

    void skybox_drawable::clear()
    {
        data.clear();
    }

    void draw(const skybox_drawable& drawable, const camera_scene& camera)
    {
        draw(drawable, camera, drawable.shader, drawable.texture_id);
    }

    void draw(const skybox_drawable& drawable, const camera_scene& camera, GLuint shader)
    {
        draw(drawable, camera, shader, drawable.texture_id);
    }

    void draw(const skybox_drawable& drawable, const camera_scene& camera, GLuint shader, GLuint texture_id)
    {
        GLenum error = 0;

        // If shader is, skip display
        if (shader == 0)
            return;

        // Check that the shader is a valid one
        if (glIsProgram(shader) == GL_FALSE)
        {
            std::cout << "No valid shader set to display mesh: skip display" << std::endl;
            return;
        }

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);

        // Switch shader program only if necessary
        GLint current_shader = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &current_shader); opengl_debug();

        if (shader != GLuint(current_shader))
            glUseProgram(shader); opengl_debug();

        // Bind texture only if id != 0
        if (texture_id != 0)
        {
            assert(glIsTexture(texture_id));
            glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);  opengl_debug();
        }

        // Send all uniform values to the shader
        //uniform(shader, "color", drawable.uniform.color);             opengl_debug();
        uniform(shader, "perspective", camera.perspective.matrix());    opengl_debug();
        uniform(shader, "view", camera.view_matrix());                  opengl_debug();

        vcl::draw(drawable.data); opengl_debug();

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }

}
