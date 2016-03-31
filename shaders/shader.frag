#version 410 core

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct DirLight {
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 4

in VS_OUT {
    vec3 fragPosition;
    vec3 Normal;
    vec2 TexCoord;
} fs_in;

out vec4 color;

uniform vec3 viewPos;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLight;
uniform Material material;

// Function prototypes
vec3 CalcPointLight(PointLight light, Material mat, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcDirLight(DirLight light, Material material, vec3 normal, vec3 viewDir);

void main()
{
    vec3 result;
    vec3 viewDir = normalize(viewPos - fs_in.fragPosition);
    vec3 norm = normalize(fs_in.Normal);
    
    result = CalcDirLight(dirLight, material, norm, viewDir);
    
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], material, norm, fs_in.fragPosition, viewDir);
    float gamma = 2.2f;
    color = vec4(pow(result, vec3(1.0/gamma)), 1.0f);
}


// Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, Material mat, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), mat.shininess); // phong-blinn
    //spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess/4); // phong
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // Combine results
    vec3 ambient = light.ambient * vec3(texture(mat.texture_diffuse1, fs_in.TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(mat.texture_diffuse1, fs_in.TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(mat.texture_specular1, fs_in.TexCoord));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcDirLight(DirLight light, Material mat, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
    // Combine results
    vec3 ambient  = light.ambient  * vec3(texture(mat.texture_diffuse1, fs_in.TexCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(mat.texture_diffuse1, fs_in.TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(mat.texture_specular1, fs_in.TexCoord));
    return (ambient + diffuse + specular);
}