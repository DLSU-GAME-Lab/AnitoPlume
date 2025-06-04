#version 330 core

in struct fragment_data
{
    vec4 position;
    vec4 normal;
    vec4 color;
    vec2 texture_uv;
} fragment;

uniform sampler2D texture_sampler;

out vec4 FragColor;

void main()
{
    vec4 color_texture = texture(texture_sampler, fragment.texture_uv);
    FragColor = color_texture;
}
