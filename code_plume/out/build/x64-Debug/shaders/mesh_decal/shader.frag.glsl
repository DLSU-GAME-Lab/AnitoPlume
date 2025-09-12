#version 330 core

in struct fragment_data
{
    vec4 position;
    vec4 normal;
    vec4 color;
    vec2 texture_uv;
    mat3 TBN;
} fragment;

//tex mesh
uniform sampler2D texture_sampler;
uniform sampler2D normal_tex;

//Texture decal
uniform sampler2D depth;
uniform sampler2D decal_tex;
uniform vec3 size;
uniform  vec2 resolution;
uniform mat4 inv_mvp;

out vec4 FragColor;

uniform vec3 camera_position;
uniform vec3 color     = vec3(1.0, 1.0, 1.0);
uniform float color_alpha = 1.0;
uniform float ambiant  = 0.2;
uniform float diffuse  = 0.8;
uniform float specular = 0.5;
uniform int specular_exponent = 128;

vec3 light = vec3(0, 150, 0);

void main()
{
    vec2 depth_uv = gl_FragCoord.xy / resolution;
	float depth = texture2D(depth, depth_uv).r;
	vec4 screenPos = vec4(depth_uv*2.0-1.0, depth*2.0-1.0, 1.0);	// clip space
	vec4 localPos = inv_mvp * screenPos;
	localPos = vec4(localPos.xyz/localPos.w, localPos.w);

    vec3 n = texture(normal_tex, fragment.texture_uv).rgb;
    n = normalize(n * 2.0 - 1.0);
    vec3 u = fragment.TBN*normalize(light-fragment.position.xyz);
    vec3 r = reflect(u,n);
    vec3 t = fragment.TBN*normalize(camera_position-fragment.position.xyz);


    float diffuse_value  = diffuse * max( dot(n,u), 0.0);
    float specular_value = specular * pow( max( dot(r,t), 0.1), specular_exponent);

    vec4 color_texture = texture(texture_sampler, depth_uv);

    vec3 white = vec3(1.0);
    vec3 c = (ambiant+diffuse_value)*color.rgb*fragment.color.rgb*color_texture.rgb + specular_value*white;

    
	if (
		abs(localPos.x) < size.x/2 &&
		abs(localPos.y) < size.y/2 &&
		abs(localPos.z) < size.z/2) 
    {
		vec2 decUv = localPos.xz/size.xz + 0.5;
		vec4 decColor = texture(decal_tex, decUv);
		FragColor = vec4(mix(c.rgb, decColor.rgb, decColor.a), color_texture.a*fragment.color.a*color_alpha);
	} 
    else 
    {
        FragColor = vec4(c, color_texture.a*fragment.color.a*color_alpha);
    }
}
