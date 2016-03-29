#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec2 TexCoord;
out vec3 fragPosition;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    fragPosition = vec3(model * vec4(position, 1.0f));
    Normal = mat3(transpose(inverse(model))) * normal;
    TexCoord = texCoord;
}


//layout (location = 0) in vec3 position;
//layout (location = 1) in vec3 normal;
//layout (location = 2) in vec2 texCoord;
//
//out vec2 TexCoord;
////out vec3 FragPos;
////out vec3 Normal;
//
//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;
//
//void main()
//{
//    gl_Position = projection * view * model * vec4(position, 1.0f);
////    FragPos = vec3(model * vec4(position, 1.0f));
////    Normal = mat3(transpose(inverse(model))) * normal;
////    TexCoord = vec2(1 - texCoord.x, texCoord.y);
//    
//    TexCoord = texCoord;
//}