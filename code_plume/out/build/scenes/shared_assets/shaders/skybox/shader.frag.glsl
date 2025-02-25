#version 330 core

out vec4 FragColor;

in vec3 tex_coords;
uniform samplerCube skybox;

void main() {
    vec3 color = texture(skybox, tex_coords).rgb;
    FragColor = vec4(color, 1.0);
}