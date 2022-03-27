#version 430 core
out vec4 f_color;

in V_OUT
{
   vec3 position;
   vec3 normal;
   vec2 texture_coordinate;
   vec3 vertex_color;
} f_in;

varying vec3 v;
varying vec3 N;

uniform vec3 u_color;

uniform sampler2D u_texture;

uniform vec3 viewPos;

uniform int shading;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 
  
uniform Material material;


//************************************************************************
//
// * Directional Light
//   
//************************************************************************
struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
uniform DirLight dirLight;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, f_in.texture_coordinate));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, f_in.texture_coordinate));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, f_in.texture_coordinate));
    return (ambient + diffuse + specular);
}


//************************************************************************
//
// * Point Light
//   
//************************************************************************
struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
#define NR_POINT_LIGHTS 4  
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, f_in.texture_coordinate));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, f_in.texture_coordinate));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, f_in.texture_coordinate));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 


//************************************************************************
//
// * Spot Light
//   
//************************************************************************
struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};
uniform SpotLight spotLight;

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, f_in.texture_coordinate));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, f_in.texture_coordinate));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, f_in.texture_coordinate));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}


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
    vec3 color = vec3(texture(u_texture, f_in.texture_coordinate));
    //color = color / 3.0f;
    if (shading == 0) //Gouraud Shading
    {
        color = f_in.vertex_color;
    }
    else if (shading == 1) //Phong Shading
    {
        vec3 L = normalize(light.position - v);   
        //vec3 L = normalize(light.position - f_in.position);
        vec3 E = normalize(-v);       // we are in Eye Coordinates, so EyePos is (0,0,0)  
        //vec3 E = normalize(-f_in.position);       // we are in Eye Coordinates, so EyePos is (0,0,0)  
        vec3 R = normalize(-reflect(L, N));  
        //vec3 R = normalize(-reflect(L, f_in.normal));  

        //calculate Ambient Term:  
        vec4 Iamb = vec4(light.ambient, 1);    

        //calculate Diffuse Term:  
        vec4 Idiff = vec4(light.diffuse * max(dot(N,L), 0.0), 1);    
        //ec4 Idiff = vec4(light.diffuse * max(dot(f_in.normal,L), 0.0), 1);    

        // calculate Specular Term:
        vec4 Ispec = vec4(light.specular * pow(max(dot(R,E),0.0),0.3*(32.0f)), 1);
        color = vec3(vec4(vec3(texture(u_texture, f_in.texture_coordinate)), 1.0f) * Iamb + Idiff + Ispec);
    }
    else //Multiple Lights
    {
        vec3 viewDir = normalize(viewPos - f_in.position);

        // phase 1: Directional lighting
        color = CalcDirLight(dirLight, f_in.normal, viewDir);
        // phase 2: Point lights
        for(int i = 0; i < NR_POINT_LIGHTS; i++)
            color += CalcPointLight(pointLights[i], f_in.normal, f_in.position, viewDir); 
        // phase 3: spot light
        color += CalcSpotLight(spotLight, f_in.normal, f_in.position, viewDir); 
    }
    f_color = vec4(color, 1.0f);
}