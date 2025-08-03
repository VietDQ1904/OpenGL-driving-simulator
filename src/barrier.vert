#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceMat;

out vec3 normal;
out vec3 fragPos;
out vec2 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main(){
   fragPos = aPos;
   normal = aNormal;   
   TexCoords = aTexCoords; 
   gl_Position = projection * view * instanceMat * vec4(aPos, 1.0);
}