#version 410 core

struct Material {
    sampler2D texture_diffuse1;
};

in vec2 TexCoord;

out vec4 color;

uniform Material u_sun_material;

void main()
{
    color = vec4(vec3(texture(u_sun_material.texture_diffuse1, TexCoord)), 1.0);
}