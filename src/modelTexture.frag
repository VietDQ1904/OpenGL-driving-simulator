#version 440 core
out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

in vec2 TexCoords;
in vec3 normal;
in vec3 fragPos;

void main(){
   FragColor = vec4(vec3(texture(texture_diffuse1, TexCoords)), 1.0);
}