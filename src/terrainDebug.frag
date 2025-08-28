#version 440 core
out vec4 FragColor;

vec3 lineColor = vec3(0.0, 1.0, 0.0);

void main(){
   FragColor = vec4(lineColor, 1.0);
}