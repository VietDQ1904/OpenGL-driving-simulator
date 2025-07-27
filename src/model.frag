#version 440 core
out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

in vec2 TexCoords;
in vec3 normal;
in vec3 fragPos;

void main(){
   
   // vec3 dir = normalize(-lightDir);
   // vec3 ambient = ambientColor * lightColor * vec3(texture(texture_diffuse1, TexCoords));
   // vec3 normalizedNorm = normalize(normal); 
   // float diff = max(dot(normalizedNorm, dir), 0.0);
   // vec3 diffuse = diffuseColor * diff * vec3(texture(texture_diffuse1, TexCoords));

   // vec3 viewDir = normalize(viewPos - fragPos);
   // vec3 reflectDir = reflect(-dir, normalizedNorm);  

   // float specularVal = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
   // vec3 specular = specularColor * specularVal * vec3(0.1); //* vec3(texture(texture_specular1, TexCoords));

   // vec3 result = ambient + diffuse + specular;

   FragColor = vec4(vec3(texture(texture_diffuse1, TexCoords)), 1.0);
}