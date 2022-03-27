#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 u_model;
//uniform mat4 lightSpaceMatrix;

layout (std140, binding = 0) uniform commom_matrices
{
    mat4 u_projection;
    mat4 u_view;
};

out V_OUT
{
   vec3 position;
   vec3 texture_coordinate;
   vec3 normal;
} v_out;

void main()
{
    v_out.texture_coordinate = position / 100.0; //100.0 is scaling
    v_out.position = vec3(u_model * vec4(position, 1.0f));
    v_out.normal = normal;
    gl_Position = u_projection * u_view * vec4(position, 1.0);
}