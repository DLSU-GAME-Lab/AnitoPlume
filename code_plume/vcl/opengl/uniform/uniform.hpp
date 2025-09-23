#pragma once

#include "vcl/math/math.hpp"
#include "vcl/wrapper/glad/glad.hpp"

#include <string>


namespace vcl
{

void set_uniform(GLuint shader, const std::string& name, const int value);
void set_uniform(GLuint shader, const std::string& name, const float value);
void set_uniform(GLuint shader, const std::string& name, const vec3& value);
void set_uniform(GLuint shader, const std::string& name, const vec4& value);
void set_uniform(GLuint shader, const std::string& name, float x, float y, float z);
void set_uniform(GLuint shader, const std::string& name, float x, float y, float z, float w);
void set_uniform(GLuint shader, const std::string& name, const mat4& m);
void set_uniform(GLuint shader, const std::string& name, const mat3& m);

}
