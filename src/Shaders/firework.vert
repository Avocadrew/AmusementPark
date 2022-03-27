#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coordinate;

uniform mat4 u_model;
//uniform vec3 u_offset;

layout (std140, binding = 0) uniform commom_matrices
{
    mat4 u_projection;
    mat4 u_view;
};

out V_OUT
{
   vec3 position;
   vec3 normal;
} v_out;

void main()
{
    gl_Position = u_projection * u_view * u_model * vec4(position, 1.0f);
    v_out.position = vec3(u_model * vec4(position, 1.0f));
    v_out.normal = mat3(transpose(u_model)) * normal;
}
