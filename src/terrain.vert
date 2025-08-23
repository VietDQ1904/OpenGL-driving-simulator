#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out MODEL_SHADER{
   vec2 TexCoords;
   vec3 normal;
   vec3 fragPos;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
   vs_out.fragPos = aPos;
   vs_out.normal = aNormal;   
   vs_out.TexCoords = aTexCoords; 
   gl_Position = projection * view * model * vec4(aPos, 1.0);
}