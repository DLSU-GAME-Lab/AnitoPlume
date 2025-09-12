#version 330 core

in struct fragment_data
{
    vec4 position;
    vec4 normal;
    vec4 color;
    vec2 texture_uv;
    vec4 clip_position;
} fragment;

uniform sampler2D depth;
uniform sampler2D texture_sampler;

out vec4 FragColor;

uniform vec3 camera_position;
uniform vec3 color     = vec3(1.0, 1.0, 1.0);
uniform float color_alpha = 1.0;
uniform float ambiant  = 0.2;
uniform float diffuse  = 0.8;
uniform float specular = 0.5;
uniform int specular_exponent = 128;
uniform float gamma = 1.0;

vec3 light = vec3(0, 0, -100);

vec3 world_position_from_depth(vec2 screen_pos, float ndc_depth)
{
    // Remap depth to [-1.0, 1.0] range.
    float depth = ndc_depth * 2.0 - 1.0;

    // // Create NDC position.
    vec4 ndc_pos = vec4(screen_pos, depth, 1.0);

    // Transform back into world position.
    //vec4 world_pos = inv_view_proj * ndc_pos;
    vec4 world_pos = vec4(0.0);

    // Undo projection.
    world_pos = world_pos / world_pos.w;

    return world_pos.xyz;
}

// ------------------------------------------------------------------

vec3 get_normal_from_map(vec3 T, vec3 B, vec3 N, vec2 tex_coord, sampler2D normal_map)
{
    // Create TBN matrix.
    mat3 TBN = mat3(normalize(T), normalize(B), normalize(N));

    // Sample tangent space normal vector from normal map and remap it from [0, 1] to [-1, 1] range.
    vec3 n = texture(normal_map, tex_coord).xyz;
    n      = normalize(n * 2.0 - 1.0);

    // Multiple vector by the TBN matrix to transform the normal from tangent space to world space.
    n = normalize(TBN * n);

    return n;
}

void main()
{
    vec2 screen_position = fragment.clip_position.xy / fragment.clip_position.w;
    vec2 tex_coords = screen_position * 0.5 + 0.5;

    vec3 n = normalize(fragment.normal.xyz);
    vec3 u = normalize(light-fragment.position.xyz);
    vec3 r = reflect(u,n);
    vec3 t = normalize(fragment.position.xyz-camera_position);


    float diffuse_value  = diffuse * clamp( dot(u,n), 0.0, 1.0);
    float specular_value = specular * pow( clamp( dot(r,t), 0.0, 1.0), specular_exponent);


    vec3 white = vec3(1.0);
    vec4 color_texture = texture(texture_sampler, fragment.texture_uv);
    vec3 c = (ambiant+diffuse_value)*color.rgb*fragment.color.rgb*color_texture.rgb + specular_value*white;

    FragColor = vec4(pow(c, vec3(1.0/gamma)), color_texture.a*fragment.color.a*color_alpha);
}
