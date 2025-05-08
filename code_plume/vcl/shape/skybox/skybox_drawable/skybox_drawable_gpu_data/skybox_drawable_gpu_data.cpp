#include "skybox_drawable_gpu_data.hpp"

#include "vcl/opengl/opengl.hpp"

namespace vcl
{

    skybox_drawable_gpu_data::skybox_drawable_gpu_data()
        :vao(0), number_triangles(0), vbo_index(0), vbo_vertices(0)
    {}

    skybox_drawable_gpu_data::skybox_drawable_gpu_data(const skybox& skybox_cpu)
    {
        // Fill VBO for vertices
        glGenBuffers(1, &vbo_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
        glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(skybox_cpu.vertices.size() * sizeof(GLfloat) * 3), &skybox_cpu.vertices[0], GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Fill VBO for index
        glGenBuffers(1, &vbo_index);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(skybox_cpu.connectivity.size() * sizeof(GLuint) * 3), &skybox_cpu.connectivity[0], GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        number_triangles = static_cast<unsigned int>(skybox_cpu.connectivity.size());

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
        glDeleteBuffers(1, &vbo_index);
    }

    void draw(const skybox_drawable_gpu_data& gpu_data)
    {
        // Doesn't draw if the structure hasn't been initialized
        if (gpu_data.number_triangles == 0 && gpu_data.vao == 0)
            return;

        if (gpu_data.number_triangles == 0)
        {
            std::cout << "Warning, try to draw data with 0 triangles" << std::endl;
            return;
        }
        assert(glIsVertexArray(gpu_data.vao));
        assert(glIsBuffer(gpu_data.vbo_index));

        glBindVertexArray(gpu_data.vao); opengl_debug();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpu_data.vbo_index); opengl_debug();
        glDrawElements(GL_TRIANGLES, GLsizei(gpu_data.number_triangles * 3), GL_UNSIGNED_INT, nullptr); opengl_debug();
        glBindVertexArray(0);
    }
}
