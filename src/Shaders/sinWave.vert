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
   vec3 normal;
   vec2 texture_coordinate;
} v_out;

uniform sampler2D u_texture;

uniform vec3 viewPos;

uniform float steepness;
uniform float wavelength;
uniform int time;
uniform vec3 waveDir;

    

void main()
{
    /*float k = 2 * 3.14159 / wavelength;
    float f = k * (position.x - time);
    vec3 sinwave = position;
    sinwave.y += steepness * sin(f);
    vec3 tangent = normalize(vec3(1,k*steepness*cos(f),0));
    gl_Position = u_projection * u_view * u_model * vec4(sinwave, 1.0f);
    v_out.position = vec3(u_model * vec4(sinwave, 1.0));
    v_out.normal = mat3(transpose(inverse(u_model))) *  normalize(vec3(-tangent.y, tangent.x, 0));
    v_out.texture_coordinate = texture_coordinate;*/
    vec3 p = position;
    float k = 2 * 3.14159 / wavelength;
    float c = sqrt(9.8 / k) / 10; //10 is scaling advise
    vec3 d = normalize(waveDir);
    float f = k * (dot(d.xz, p.xz) - c * time);
    float a = steepness / k;
    p.x += d.x * a * cos(f);
    p.y = a * sin(f);
    p.z += d.z * a * cos(f);
    v_out.position = vec3(u_model * vec4(p, 1.0f));

    vec3 tangent = normalize(vec3(1 - d.x * d.x * steepness * sin(f),
                                  d.x * steepness * cos(f),
                                  -d.x * d.y * steepness * sin(f)));
    vec3 binormal = vec3(-d.x * d.z * steepness * sin(f),
				          d.z * steepness * cos(f),
				          1 - d.z * d.z * steepness * sin(f));
    v_out.normal = normalize(cross(binormal, tangent));

    v_out.texture_coordinate = vec2(texture_coordinate.x, texture_coordinate.y);

    gl_Position = u_projection * u_view * vec4(v_out.position, 1.0f);
}