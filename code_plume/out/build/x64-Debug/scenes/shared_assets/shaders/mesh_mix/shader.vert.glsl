#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 normal;
layout (location = 2) in vec4 color;
layout (location = 3) in vec2 texture_uv;
layout (location = 4) in vec3 tangent;
layout (location = 5) in vec3 bitangent;

out struct fragment_data
{
    vec4 position;
    vec4 normal;
    vec4 color;
    vec2 texture_uv;
    mat3 TBN;
} fragment;


// model transformation
uniform vec3 translation = vec3(0.0, 0.0, 0.0);                      // user defined translation
uniform mat3 rotation = mat3(1.0,0.0,0.0, 0.0,1.0,0.0, 0.0,0.0,1.0); // user defined rotation
uniform float scaling = 1.0;                                         // user defined scaling
uniform vec3 scaling_axis = vec3(1.0,1.0,1.0);                       // user defined scaling


// view transform
uniform mat4 view;
// perspective matrix
uniform mat4 perspective;



void main()
{
    // scaling matrix
    mat4 S = mat4(scaling*scaling_axis.x,0.0,0.0,0.0, 0.0,scaling*scaling_axis.y,0.0,0.0, 0.0,0.0,scaling*scaling_axis.z,0.0, 0.0,0.0,0.0,1.0);
    // 4x4 rotation matrix
    mat4 R = mat4(rotation);
    // 4D translation
    vec4 T = vec4(translation,0.0);
    // RS matrix
    mat4 model = R*S;

    vec3 t = normalize(vec3(model * vec4(tangent,   0.0)));
    vec3 b = normalize(vec3(model * vec4(bitangent, 0.0)));
    vec3 n = normalize(vec3(model * normal));
    mat3 TBN = transpose(mat3(t, b, n));
//
//    float depth = 1;
//    vec3 cam_to_frag;
//    vec3 cam_direction;
//    vec3 euclidean = depth / (cam_to_frag * cam_direction);
//    float F = euclidean / (cam_to_frag.y - cam_direction.y)
//
    fragment.TBN = TBN;
    fragment.color = color;
    fragment.texture_uv = texture_uv;

    fragment.normal = R*normal;
    vec4 position_transformed = model*position + T;

    fragment.position = position_transformed;
    gl_Position = perspective * view * position_transformed;
}
