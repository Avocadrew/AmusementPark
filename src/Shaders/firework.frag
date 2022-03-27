#version 430 core
out vec4 f_color;

in V_OUT
{
   vec3 position;
   vec3 normal;
} f_in;

layout (std140, binding = 0) uniform commom_matrices
{
    mat4 u_projection;
    mat4 u_view;
};
uniform samplerCube skybox;
uniform int u_type;
uniform vec4 u_color;
void main()
{ 
    if (u_type==0)
    {
        mat4 viewInv = inverse(u_view);
        vec3 camaraPos = vec3(viewInv[3][0],viewInv[3][1],viewInv[3][2]);
        vec3 I = normalize(f_in.position - camaraPos);
        vec3 R = reflect(I, normalize(f_in.normal));
        vec3 RL = refract(I, normalize(f_in.normal), 1.0/1.23);
        vec3 colorReflec = vec3(texture(skybox, R).rgb);
        vec3 colorRefrac = vec3(texture(skybox, RL).rgb);
        vec3 color = mix(colorRefrac, colorReflec, 0.3);
        f_color = vec4(color, 1.0f);
    }
    else if(u_type==1){
        f_color = vec4(u_color);
    }
}