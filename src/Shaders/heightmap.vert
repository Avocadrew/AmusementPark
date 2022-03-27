#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texture_coordinate;

uniform mat4 u_model;

layout (std140, binding = 0) uniform commom_matrices
{
    mat4 u_projection;
    mat4 u_view;
};

out V_OUT
{
    vec3 position;
    vec2 texture_coordinate;
    vec2 heightMap_coordinate;
} v_out;

uniform sampler2D u_texture;
uniform sampler2D heightMap;

uniform vec3 viewPos;
uniform float amplitude;
uniform vec2 u_delta;
const float PI = 3.14159;

void main()
{
    vec3 p = position;
    v_out.heightMap_coordinate = vec2((p.x / 100.0 + 1.0f) / 2.0f, (p.z / 100.0 + 1.0f) / 2.0f); //100.0 is scaling

    vec3 info = texture(heightMap, v_out.heightMap_coordinate).rgb;

    p.y += 5 * 1 + amplitude * info.r - (p.x - p.z)/2;

    v_out.position = vec3(u_model * vec4(p, 1.0f));

    v_out.texture_coordinate = vec2(texture_coordinate.x, texture_coordinate.y);

    gl_Position = u_projection * u_view * vec4(v_out.position, 1.0f);
}