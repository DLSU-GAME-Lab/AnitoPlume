#version 330 core

in struct fragment_data
{
    vec4 position;
    vec4 normal;
    vec4 color;
    vec2 texture_uv;
} fragment;

uniform sampler2D texture_sampler;

uniform vec3 camera_position;

uniform vec3 fog_color = vec3(0.5, 0.5, 0.5);
uniform float fog_density = 0.001;
uniform float fog_fade_height = 20.0;
uniform float fog_max_height = 30.0;

out vec4 FragColor;

void main()
{
    vec4 color_texture = texture(texture_sampler, fragment.texture_uv);
    
    // fog
    float fog_factor = 0.0;
    if (fog_density > 0)
    {
        float fog_distance = length(vec3(fragment.position) - camera_position);
        float fog_factor = exp(-fog_density * fog_distance);
        fog_factor = clamp(fog_factor, 0.0, 1.0);
    
        if (fragment.position.z >= fog_fade_height && fragment.position.z <= fog_max_height)
        {
            float height_factor = (fragment.position.z - fog_fade_height) / (fog_max_height - fog_fade_height);
            fog_factor += height_factor;
            fog_factor = clamp(fog_factor, 0.0, 1.0);
        }
        else if (fragment.position.z > fog_max_height) fog_factor = 1.0;
    }
    
    if (fog_factor < 0.01)
        FragColor = color_texture;
    else FragColor = vec4(mix(fog_color, color_texture.rgb, fog_factor), color_texture.a);
}
