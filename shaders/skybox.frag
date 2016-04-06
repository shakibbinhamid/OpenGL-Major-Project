#version 410 core

in vec3 TexCoords;
out vec4 color;

uniform samplerCube skybox;
uniform vec4 uColor;

void main()
{
    color = texture(skybox, TexCoords) * uColor;
}