#version 430 core

#extension GL_NV_shadow_samplers_cube : enable

out vec4 f_color;

uniform mat4 u_model;

in V_OUT
{
   vec3 position;
   vec3 texture_coordinate;
   vec3 normal;
} f_in;

uniform samplerCube tiles;


uniform vec3 viewPos;


void main()
{   
    vec3 viewDir = normalize(f_in.position - viewPos);
    vec3 normal = normalize(mat3(transpose(inverse(u_model))) * f_in.normal);
    float cosTheta = dot(viewDir, normal) / length(viewDir) / length(normal);
    if (cosTheta < 0) discard;
    vec3 color = texture(tiles, f_in.texture_coordinate).rgb;
    f_color = vec4(color, 1.0);
}