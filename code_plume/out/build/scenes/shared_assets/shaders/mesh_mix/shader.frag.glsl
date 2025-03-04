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
uniform float blend_progress = 0.f;
uniform int specular_exponent = 128;

vec3 light = vec3(0, 0, -100);

void main()
{
    //vec3 n = normalize(fragment.normal.xyz);
    vec3 n = texture(normal_tex, fragment.texture_uv).rgb;
    n = normalize(n * 2.0 - 1.0);
//    n = normalize(fragment.TBN*n);
    vec3 u = fragment.TBN*normalize(light-fragment.position.xyz);
    vec3 r = reflect(u,n);
    vec3 t = fragment.TBN*normalize(camera_position-fragment.position.xyz);
    //vec3 fragPos = vec3(fragment.position.x, fragment.position.y, fragment.position.z);
    //vec3 lightDir = fragment.TBN * normalize(light - fragment.position);
    //vec3 viewPos = fragment.TBN * normalize(camera_position - fragment.position);
//    light = fragment.TBN * light;


    float diffuse_value  = diffuse * max( dot(n,u), 0.0);
    float specular_value = specular * pow( max( dot(r,t), 0.1), specular_exponent);


    vec3 white = vec3(1.0);
    vec4 color_texture = texture(texture_sampler, fragment.texture_uv);
    vec4 blend_texture = texture(texture_blend, fragment.texture_uv);
    vec3 c = (ambiant+diffuse_value)*color.rgb*fragment.color.rgb*color_texture.rgb + specular_value*white;

    FragColor = vec4(mix(c, blend_texture.rgb, blend_progress), color_texture.a*fragment.color.a*color_alpha);
}
