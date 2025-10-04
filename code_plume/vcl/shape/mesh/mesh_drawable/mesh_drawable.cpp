#include "mesh_drawable.hpp"

#include "vcl/opengl/opengl.hpp"

namespace vcl
{



mesh_drawable::mesh_drawable()
    :data(),uniform(),shader(0),texture_id(0),norm_tex_id(0)
{}

mesh_drawable::mesh_drawable(const mesh& mesh_arg, GLuint shader_arg, GLuint texture_id_arg, GLuint norm_tex_id_arg)
    :data(mesh_arg),uniform(),shader(shader_arg),texture_id(texture_id_arg),norm_tex_id(norm_tex_id_arg)
{}

void mesh_drawable::clear()
{
    data.clear();
}

void mesh_drawable::update_position(const vcl::buffer<vec3>& new_position)
{
    data.update_position(new_position);
}

void mesh_drawable::update_normal(const vcl::buffer<vec3>& new_normal)
{
    data.update_normal(new_normal);
}

void mesh_drawable::draw(const camera_scene& camera, GLuint shader, GLuint texture_id, GLuint norm_tex_id)
{
    // If shader is, skip display
    if (shader == 0)
    {
        std::cout << "display  skipped" << std::endl;
        return;
    }

    // Check that the shader is a valid one
    if (glIsProgram(shader) == GL_FALSE) {
        std::cout << "No valid shader set to display mesh: skip display" << std::endl;
        return;
    }

    // Switch shader program only if necessary
    GLint current_shader = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current_shader); opengl_debug();
    if (shader != GLuint(current_shader))
        glUseProgram(shader); opengl_debug();

    // Bind texture only if id != 0
    if (texture_id != 0)
    {
        assert(glIsTexture(texture_id));
        //glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id);  opengl_debug();
    }

    if (norm_tex_id != 0)
    {
        assert(glIsTexture(norm_tex_id));
        //glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, norm_tex_id);  opengl_debug();
    }
  

    // Send all uniform values to the shader
    set_uniform(shader, "rotation", uniform.transform.rotation);            opengl_debug();
    set_uniform(shader, "translation", uniform.transform.translation);      opengl_debug();
    set_uniform(shader, "color", uniform.color);                            opengl_debug();
    set_uniform(shader, "color_alpha", uniform.color_alpha);                opengl_debug();
    set_uniform(shader, "scaling", uniform.transform.scaling);              opengl_debug();
    set_uniform(shader, "scaling_axis", uniform.transform.scaling_axis);    opengl_debug();

    set_uniform(shader, "perspective", camera.perspective.matrix());         opengl_debug();
    set_uniform(shader, "view", camera.view_matrix());                       opengl_debug();
    set_uniform(shader, "camera_position", camera.camera_position());        opengl_debug();

    set_uniform(shader, "ambiant", uniform.shading.ambiant);      opengl_debug();
    set_uniform(shader, "diffuse", uniform.shading.diffuse);      opengl_debug();
    set_uniform(shader, "specular", uniform.shading.specular);    opengl_debug();
    set_uniform(shader, "specular_exponent", uniform.shading.specular_exponent); opengl_debug();

    set_uniform(shader, "gamma", camera.gamma);                     opengl_debug();
    set_uniform(shader, "fog_color", camera.fog_color);             opengl_debug();
    set_uniform(shader, "fog_start", camera.fog_start);             opengl_debug();
    set_uniform(shader, "fog_density", camera.fog_density);         opengl_debug();
    set_uniform(shader, "fog_fade_height", camera.fog_fade_height); opengl_debug();
    set_uniform(shader, "fog_max_height", camera.fog_max_height);   opengl_debug();

    data.draw(); opengl_debug();
}

void mesh_drawable::draw_sky(const camera_scene& camera, GLuint shader, GLuint texture_id)
{
    // If shader is, skip display
    if (shader == 0)
    {
        std::cout << "display  skipped" << std::endl;
        return;
    }

    // Check that the shader is a valid one
    if (glIsProgram(shader) == GL_FALSE)
    {
        std::cout << "No valid shader set to display mesh: skip display" << std::endl;
        return;
    }

    // Switch shader program only if necessary
    GLint current_shader = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current_shader); opengl_debug();
    if (shader != GLuint(current_shader))
        glUseProgram(shader); opengl_debug();

    if (texture_id != 0)
    {
        assert(glIsTexture(texture_id));
        //glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id);  opengl_debug();
    }

    set_uniform(shader, "rotation", uniform.transform.rotation);         opengl_debug();
    set_uniform(shader, "translation", uniform.transform.translation);   opengl_debug();
    set_uniform(shader, "scaling", uniform.transform.scaling);           opengl_debug();
    set_uniform(shader, "scaling_axis", uniform.transform.scaling_axis); opengl_debug();

    set_uniform(shader, "perspective", camera.perspective.matrix());              opengl_debug();
    set_uniform(shader, "view", camera.view_matrix());                            opengl_debug();

    set_uniform(shader, "gamma", camera.gamma);                     opengl_debug();
    set_uniform(shader, "fog_color", camera.fog_color);             opengl_debug();
    set_uniform(shader, "fog_start", camera.fog_start);             opengl_debug();
    set_uniform(shader, "fog_density", camera.fog_density);         opengl_debug();
    set_uniform(shader, "fog_fade_height", camera.fog_fade_height); opengl_debug();
    set_uniform(shader, "fog_max_height", camera.fog_max_height);   opengl_debug();

    data.draw(); opengl_debug();
}

void mesh_drawable::draw_mix(const camera_scene& camera, GLuint shader, GLuint texture_id, GLuint norm_tex_id, GLuint mix_tex_id, float mix_prog)
{
    // If shader is, skip display
    if (shader == 0)
    {
        std::cout << "display  skipped" << std::endl;
        return;
    }

    // Check that the shader is a valid one
    if (glIsProgram(shader) == GL_FALSE) {
        std::cout << "No valid shader set to display mesh: skip display" << std::endl;
        return;
    }

    // Switch shader program only if necessary
    GLint current_shader = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current_shader); opengl_debug();
    if (shader != GLuint(current_shader))
        glUseProgram(shader); opengl_debug();

    // Bind texture only if id != 0
    //if (norm_tex_id != 0)
    //{
    //    assert(glIsTexture(norm_tex_id));
    //    //glActiveTexture(GL_TEXTURE1);
    //    glBindTexture(GL_TEXTURE_2D, norm_tex_id);  opengl_debug();
    //}
    if (texture_id != 0) {
        assert(glIsTexture(texture_id));
        //glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id);  opengl_debug();
    }
    //if (mix_tex_id != 0)
    //{
    //    assert(glIsTexture(mix_tex_id));
    //    //glActiveTexture(GL_TEXTURE0);
    //    glBindTexture(GL_TEXTURE_2D, mix_tex_id);  opengl_debug();
    //}
    set_uniform(shader, "rotation", uniform.transform.rotation);           opengl_debug();
    set_uniform(shader, "translation", uniform.transform.translation);     opengl_debug();
    set_uniform(shader, "color", uniform.color);                           opengl_debug();
    set_uniform(shader, "color_alpha", uniform.color_alpha);               opengl_debug();
    set_uniform(shader, "scaling", uniform.transform.scaling);             opengl_debug();
    set_uniform(shader, "scaling_axis", uniform.transform.scaling_axis);   opengl_debug();

    set_uniform(shader, "perspective", camera.perspective.matrix());    opengl_debug();
    set_uniform(shader, "view", camera.view_matrix());                  opengl_debug();
    set_uniform(shader, "camera_position", camera.camera_position());   opengl_debug();

    set_uniform(shader, "ambiant", uniform.shading.ambiant);                       opengl_debug();
    set_uniform(shader, "diffuse", uniform.shading.diffuse);                       opengl_debug();
    set_uniform(shader, "specular", uniform.shading.specular);                     opengl_debug();
    set_uniform(shader, "specular_exponent", uniform.shading.specular_exponent);   opengl_debug();

    set_uniform(shader, "gamma", camera.gamma);                     opengl_debug();
    set_uniform(shader, "fog_color", camera.fog_color);             opengl_debug();
    set_uniform(shader, "fog_start", camera.fog_start);             opengl_debug();
    set_uniform(shader, "fog_density", camera.fog_density);         opengl_debug();
    set_uniform(shader, "fog_fade_height", camera.fog_fade_height); opengl_debug();
    set_uniform(shader, "fog_max_height", camera.fog_max_height);   opengl_debug();

    data.draw(); opengl_debug();

}

}
