#version 440 core
out vec4 FragColor;

in MODEL_SHADER{
   vec2 TexCoords;
   vec3 normal;
   vec3 fragPos;
} vs_out;

struct DirLight{
   vec3 direction;
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
};

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_metallic1;
uniform sampler2D texture_normal1;
uniform vec3 viewPos;
DirLight light;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir){
   vec3 lightDir = normalize(-light.direction);
   float diff = max(dot(normal, lightDir), 0.0);
   
   vec3 reflectDir = reflect(-lightDir, normal);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

   vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, vs_out.TexCoords));
   vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, vs_out.TexCoords));
   vec3 specular = light.specular * spec * vec3(texture(texture_specular1, vs_out.TexCoords));
   
   return (ambient + diffuse + specular);
}

void main(){
   light.direction = vec3(-1.0, -2.0, -2.0);
   light.ambient = vec3(0.3, 0.3, 0.3);
   light.diffuse = vec3(0.4, 0.4, 0.4);
   light.specular = vec3(0.5, 0.5, 0.5);

   vec3 viewDir = normalize(viewPos - vs_out.fragPos);
   vec3 normalizedNormal = normalize(vs_out.normal);
   vec3 ambient = vec3(texture(texture_diffuse1, vs_out.TexCoords));

   vec3 result = calcDirLight(light, normalizedNormal, viewDir);
   
   FragColor = vec4(result, 1.0);
}