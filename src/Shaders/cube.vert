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
   vec3 vertex_color;
} v_out;

varying vec3 v;
varying vec3 N;

uniform sampler2D u_texture;

uniform int shading;


//************************************************************************
//
// * Light to Gouraud Shading & Phong Shading
//   
//************************************************************************
struct Light {    
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

void main()
{
    v_out.position = vec3(u_model * vec4(position, 1.0f));
    v_out.normal = normalize(mat3(transpose(inverse(u_model))) * normal);
    v_out.texture_coordinate = vec2(texture_coordinate.x, 1.0f - texture_coordinate.y);

    if (shading == 0) //Gouraud Shading
    {
        vec3 L = normalize(light.position - v_out.position);   
        vec3 E = normalize(-v_out.position);       // we are in Eye Coordinates, so EyePos is (0,0,0)  
        vec3 R = normalize(-reflect(L, v_out.normal));  

        //calculate Ambient Term:  
        vec4 Iamb = vec4(light.ambient, 1);    

        //calculate Diffuse Term:  
        vec4 Idiff = vec4(light.diffuse * max(dot(v_out.normal,L), 0.0), 1);    

        // calculate Specular Term:
        vec4 Ispec = vec4(light.specular * pow(max(dot(R,E),0.0),0.3*(32.0f)), 1);
        v_out.vertex_color = vec3(texture(u_texture, v_out.texture_coordinate) * Iamb + Idiff + Ispec);
    }
    else if (shading == 1) //Phong shading
    {
        v = vec3(u_model * vec4(position, 1.0f));      
        N = normalize(mat3(transpose(inverse(u_model))) * normal);
    }

    gl_Position = u_projection * u_view * vec4(v_out.position, 1.0f);
}