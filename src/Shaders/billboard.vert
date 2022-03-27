#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coordinate;

uniform mat4 u_model;

layout (std140, binding = 0) uniform commom_matrices
{
    mat4 u_projection;
    mat4 u_view;
};

out V_OUT
{
   vec3 position;
   vec3 normal;
   vec2 texture_coordinate;
} v_out;
uniform float u_trainU;

void main()
{
    // reference: http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/billboards/#solution-2--the-3d-way
    vec3 right = vec3(u_view[0][0], u_view[1][0], u_view[2][0]);
    vec3 up = vec3(u_view[0][1], u_view[1][1], u_view[2][1]);
    vec3 pos = vec3(-0.5,1.5,-0.25)+right * (texture_coordinate.x) + up * (texture_coordinate.y);
    gl_Position =  u_projection * u_view * u_model * vec4(pos, 1.0f);
    v_out.position = vec3(u_model * vec4(position, 1.0f));
    v_out.normal = mat3(transpose(inverse(u_model))) * normal;
    v_out.texture_coordinate = vec2(1-texture_coordinate.x, 1 - texture_coordinate.y);
}