#include "skybox_drawable_gpu_data.hpp"

#include "vcl/opengl/opengl.hpp"

namespace vcl
{

    skybox_drawable_gpu_data::skybox_drawable_gpu_data()
        :vao(0), vbo_vertices(0)
    {}

    skybox_drawable_gpu_data::skybox_drawable_gpu_data(const skybox& skybox_cpu)
    {
        // Fill VBO for vertices
        glGenBuffers(1, &vbo_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
        glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(skybox_cpu.vertices.size() * sizeof(GLfloat) * 3), &skybox_cpu.vertices[0], GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // vertices at layout 0
        glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void skybox_drawable_gpu_data::clear()
    {
        glDeleteBuffers(1, &vbo_vertices);
    }

    void draw(const skybox_drawable_gpu_data& gpu_data)
    {
        assert(glIsVertexArray(gpu_data.vao));

        glBindVertexArray(gpu_data.vao); opengl_debug();
        glBindBuffer(GL_ARRAY_BUFFER, gpu_data.vbo_vertices); opengl_debug();

        glDrawArrays(GL_TRIANGLES, 0, 36); opengl_debug();
        glBindVertexArray(0);
    }



}
