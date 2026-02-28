#version 330 core
layout (location = 0) in vec4 position;
layout (location = 1) in vec4 normal;
layout (location = 2) in vec4 color;
layout (location = 3) in vec2 texture_uv;

out struct fragment_data
{
    vec4 position;
    vec4 normal;
    vec4 color;
    vec2 texture_uv;
    vec4 clip_position;
} fragment;

uniform vec3 translation = vec3(0.0, 0.0, 0.0);                      // user defined translation
uniform mat3 rotation = mat3(1.0,0.0,0.0, 0.0,1.0,0.0, 0.0,0.0,1.0); // user defined rotation
uniform float scaling = 1.0;                                         // user defined scaling
uniform vec3 scaling_axis = vec3(1.0,1.0,1.0);                       // user defined scaling

uniform mat4 model; // Model matrix for the decal
uniform mat4 view;  // View matrix
uniform mat4 perspective; // Projection matrix

out vec3 FragPos; // Fragment position in world space
out vec3 Normal; // Normal vector in world space
out vec2 TexCoords; // Texture coordinates

void main()
{
    // scaling matrix
    mat4 S = mat4(scaling*scaling_axis.x,0.0,0.0,0.0, 0.0,scaling*scaling_axis.y,0.0,0.0, 0.0,0.0,scaling*scaling_axis.z,0.0, 0.0,0.0,0.0,1.0);
    // 4x4 rotation matrix
    mat4 R = mat4(rotation);
    // 4D translation
    vec4 T = vec4(translation,0.0);


    fragment.color = color;
    fragment.texture_uv = texture_uv;

    fragment.normal = R*normal;
    vec4 position_transformed = R*S*position + T;

    fragment.position = position_transformed;
    gl_Position = perspective * view * position_transformed;
    fragment.clip_position = gl_Position;
}
