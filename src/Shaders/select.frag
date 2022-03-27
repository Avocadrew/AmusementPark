#version 430 core

#extension GL_NV_shadow_samplers_cube : enable

out vec4 f_color;

in V_OUT
{
    vec3 position;
    vec2 texture_coordinate;
    vec2 heightMap_coordinate;
} f_in;

uniform vec3 u_color;

uniform sampler1D tex_toon;
uniform sampler2D u_texture;
uniform sampler2D heightMap;
uniform samplerCube skybox;
uniform samplerCube environmentbox;
uniform vec2 u_delta;

uniform vec3 viewPos;

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

uniform float Eta = 1 / 1.33;
uniform float amplitude;
uniform bool cartoon;

float cartoonRG(float x)
{
    float neer_color[] = { 0, 36 / 256.0, 73 / 256.0, 109 / 256.0, 146 / 256.0, 182 / 256.0, 219 / 256.0, 1.0};
    int neer_color_num = 8;
    for (int neer_color_index = 0; neer_color_index < neer_color_num - 1; neer_color_index++)
    {
        if (neer_color[neer_color_index] <= x && x < neer_color[neer_color_index + 1])
        {
            if (x - neer_color[neer_color_index] < neer_color[neer_color_index + 1] - x)
            {
                return neer_color[neer_color_index];
            }
            else
            {
                return neer_color[neer_color_index + 1];
            }
        }
    }
    return neer_color[neer_color_num - 1];
}

float cartoonB(float x)
{
    float neer_color[] = { 0, 85 / 256.0, 170 / 256.0, 1.0 };
    int neer_color_num = 4;
    for (int neer_color_index = 0; neer_color_index < neer_color_num - 1; neer_color_index++)
    {
        if (neer_color[neer_color_index] <= x && x < neer_color[neer_color_index + 1])
        {
            if (x - neer_color[neer_color_index] < neer_color[neer_color_index + 1] - x)
            {
                return neer_color[neer_color_index];
            }
            else
            {
                return neer_color[neer_color_index + 1];
            }
        }
    }
    return neer_color[neer_color_num - 1];
}

uniform vec3 environmentCenter;
uniform vec3 bboxMax;
uniform vec3 bboxMin;
vec3 LocalCorrect(vec3 origVec, vec3 vertexPos)
{
    //*********************************************************************
	// Author: Roberto Lopez Mendez
	// URL: https://community.arm.com/developer/tools-software/graphics/b/blog/posts/reflections-based-on-local-cubemaps-in-unity
	//*********************************************************************

    // Find the ray intersection with box plane

    vec3 invOrigVec = vec3(1.0,1.0,1.0)/origVec;

    vec3 intersecAtMaxPlane = (bboxMax - vertexPos) * invOrigVec;

    vec3 intersecAtMinPlane = (bboxMin - vertexPos) * invOrigVec;

    // Get the largest intersection values (we are not intersted in negative values)

    vec3 largestIntersec = max(intersecAtMaxPlane, intersecAtMinPlane);

    // Get the closest of all solutions

   float Distance = min(min(largestIntersec.x, largestIntersec.y), largestIntersec.z);

    // Get the intersection position

    vec3 IntersectPositionWS = vertexPos + origVec * Distance;

    // Get corrected vector

    vec3 localCorrectedVec = IntersectPositionWS - environmentCenter;

    return localCorrectedVec;
}

uniform int showRleRra;

struct Light {
    vec3 direction;
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
    float cutOff;
};

uniform Light light;

void main()
{
    vec3 dx = vec3(
        200 * u_delta.y, //[-100, 100] in x
        (texture(heightMap, vec2(f_in.heightMap_coordinate.x, f_in.heightMap_coordinate.y + u_delta.y)).r - texture(heightMap, f_in.heightMap_coordinate).r) * amplitude, 
        0.0);
    vec3 dz = vec3(
        0.0,
        (texture(heightMap, vec2(f_in.heightMap_coordinate.x + u_delta.x, f_in.heightMap_coordinate.y)).r - texture(heightMap, f_in.heightMap_coordinate).r) * amplitude,
        200 * u_delta.x); //[-100, 100] in z
    vec3 normal = normalize(cross(dz, dx));

  
    vec3 InVec = normalize(f_in.position - viewPos); 
    //湖面（各頂點）到眼睛的入射向量
 
    vec3 ReflectVec = reflect(InVec, normal);
    vec3 RefractVec;
    float cosTheta = dot(InVec, normal) / length(InVec) / length(normal);
    if (cosTheta > 0)
        RefractVec = refract(InVec, -normal, 1.33); //Eta，折射率，1.33
    else
        RefractVec = refract(InVec, normal, Eta); //Eta，折射率，1 / 1.33
    vec3 newReflectVec = LocalCorrect(ReflectVec, f_in.position);
    vec3 newRefractVec = LocalCorrect(RefractVec, f_in.position);
    vec3 ReflectColor = vec3(texture(environmentbox, newReflectVec));
    vec3 RefractColor = vec3(texture(environmentbox, newRefractVec));

    float ratio = 0.4 + 0.6 * pow(min(0.0, 1.0 - dot(-InVec, normal)), 4.0); //float fresnel 

    vec3 color;
    if (showRleRra == 0 || showRleRra == 3)
        color = mix(RefractColor, ReflectColor, ratio);
    else if (showRleRra == 2)
        color = ReflectColor;
    else if (showRleRra == 1)
        color = RefractColor;
    
//    vec3 viewDir = normalize(viewPos - f_in.position);
//    // phase 1: Directional lighting
//    color += CalcDirLight(dirLight, normal, viewDir);
//    // phase 2: Point lights
//    for(int i = 0; i < NR_POINT_LIGHTS; i++)
//        color += CalcPointLight(pointLights[i], normal, f_in.position, viewDir); 
//    // phase 3: spot light
//    color += CalcSpotLight(spotLight, normal, f_in.position, viewDir); 

////
    // PureLighting
    ////

    //ambient lighting
    vec3 color1 = vec3(0,0.5,0.7);
    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
    float ambientStrength = 0.7;
    vec3 ambient = ambientStrength * lightColor;

    //diffuse lighting
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    //specular lighting
    float specularStrength = 3;
    vec3 viewDir = normalize(viewPos - f_in.position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = specularStrength * spec * lightColor;
    if(diff <= 0)
        specular = vec3(0.0f, 0.0f, 0.0f);

    vec3 result = (ambient + diffuse + specular) * color1;



    if (cartoon) color = vec3(cartoonRG(color.r), cartoonRG(color.g), cartoonB(color.b));
    vec3 colorX = vec3(texture(u_texture, f_in.texture_coordinate));
    f_color = vec4(colorX, 1.0);
}