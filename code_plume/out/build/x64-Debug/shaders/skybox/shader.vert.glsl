#version 330 core

layout(location = 0) in vec3 position;

out vec3 tex_coords;

uniform mat4 perspective;
uniform mat4 view;

void main() {
    vec4 pos = perspective * view * vec4(position, 1.0);
    tex_coords = vec3(pos);
    gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);
}