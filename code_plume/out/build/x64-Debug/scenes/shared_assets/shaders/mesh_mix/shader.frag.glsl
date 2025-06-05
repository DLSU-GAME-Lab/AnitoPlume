#version 330 core

in struct fragment_data
{
    vec4 position;
    vec4 normal;
    vec4 color;
    vec2 texture_uv;
    mat3 TBN;
} fragment;

uniform sampler2D texture_sampler;
uniform sampler2D texture_blend;
uniform sampler2D normal_tex;

out vec4 FragColor;

uniform vec3 camera_position;
uniform vec3 color     = vec3(1.0, 1.0, 1.0);
uniform float color_alpha = 1.0;
uniform float ambiant  = 0.2;
uniform float diffuse  = 0.8;
uniform float specular = 0.5;
uniform int specular_exponent = 128;
uniform float blend_progress = 0.0;

uniform vec3 fog_color = vec3(0.5, 0.5, 0.5);
uniform float fog_start = 20.0;
uniform float fog_density = 0.001;
uniform float fog_fade_height = 20.0;
uniform float fog_max_height = 30.0;

vec3 light = vec3(0, 0, -100);

void main()
{
//    vec3 n = normalize(fragment.normal.xyz);
//    vec3 u = normalize(light-fragment.position.xyz);
//    vec3 r = reflect(u,n);
//    vec3 t = normalize(fragment.position.xyz-camera_position);
//
//    float diffuse_value  = diffuse * clamp( dot(u,n), 0.0, 1.0);
//    float specular_value = specular * pow( clamp( dot(r,t), 0.0, 1.0), specular_exponent);

    vec3 n = normalize(texture(normal_tex, fragment.texture_uv).rgb * 2.0 - 1.0);
    vec3 u = fragment.TBN*normalize(light-fragment.position.xyz);
    vec3 r = reflect(u,n);
    vec3 t = fragment.TBN*normalize(camera_position-fragment.position.xyz);

    float diffuse_value  = diffuse * max( dot(n,u), 0.0);
    float specular_value = specular * pow( max( dot(r,t), 0.1), specular_exponent);


    // texture colors
    vec3 white = vec3(1.0);
    vec4 color_texture = texture(texture_sampler, fragment.texture_uv);
    //vec4 blend_texture = texture(texture_blend, fragment.texture_uv);
    
    vec3 base_color = (ambiant+diffuse_value)*color.rgb*fragment.color.rgb*color_texture.rgb + specular_value*white;
    //vec3 blend_color = mix(base_color, blend_texture.rgb, blend_progress);

    // fog
    float fog_factor = 0.0;
    if (fog_density > 0)
    {
        float fog_distance = length(vec3(fragment.position) - camera_position);
        fog_factor = exp(-fog_density * (fog_distance - fog_start));
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
        FragColor = vec4(base_color, color_texture.a*fragment.color.a*color_alpha);
    else FragColor = vec4(mix(fog_color, base_color, fog_factor), color_texture.a*fragment.color.a*color_alpha);
}
