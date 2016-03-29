#version 410 core



struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};
/* Note: because we now use a material struct again you want to change your
 mesh class to bind all the textures using material.texture_diffuseN instead of
 texture_diffuseN. */

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 2

in vec3 fragPosition;
in vec3 Normal;
in vec2 TexCoord;

out vec4 color;

uniform vec3 viewPos;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;

// Function prototypes
vec3 CalcPointLight(PointLight light, Material mat, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 result;
    vec3 viewDir = normalize(viewPos - fragPosition);
    vec3 norm = normalize(Normal);
    
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], material, norm, fragPosition, viewDir);
    
    color = vec4(result, 1.0f);
}


// Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, Material mat, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // Combine results
    vec3 ambient = light.ambient * vec3(texture(mat.texture_diffuse1, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(mat.texture_diffuse1, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(mat.texture_specular1, TexCoord));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}





//in vec2 TexCoord;
//
//out vec4 color;
//
//uniform sampler2D texture_diffuse1;
//
//void main() {
//    color = vec4(texture(texture_diffuse1, TexCoord));
//}








//struct Material {
//    sampler2D diffuse;
//    sampler2D specular;
//    float shininess;
//};
//
//struct Light {
//    vec3 position;
//    
//    vec3 ambient;
//    vec3 diffuse;
//    vec3 specular;
//    
//    float constant;
//    float linear;
//    float quadratic;
//};
//
//in vec3 Normal;
//in vec3 FragPos;
//in vec2 TexCoord;
//
//out vec4 color;
//
//uniform Material material;
//uniform Light light;
//uniform vec3 viewPos;
//
//void main() {
//    // Ambient
//    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
//    
//    // Diffuse
//    vec3 norm = normalize(Normal);
//    vec3 lightDir = normalize(light.position - FragPos);
//    float diff = max(dot(norm, lightDir), 0.0);
//    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
//    
//    // Specular
//    vec3 viewDir = normalize(viewPos - FragPos);
//    vec3 reflectDir = reflect(-lightDir, norm);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
//    
//    // Attenuation
//    float distance    = length(light.position - FragPos);
//    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
//    
//    ambient  *= attenuation;
//    diffuse  *= attenuation;
//    specular *= attenuation;
//    
//    color = vec4(ambient + diffuse + specular, 1.0f);
//}