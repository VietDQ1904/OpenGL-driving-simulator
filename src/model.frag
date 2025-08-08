#version 330 core
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
uniform DirLight light;

// Generate a pseudo-random number.
float hash(vec2 p) {
   return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

// Rotate UV map
vec2 rotateUV(vec2 uv, float angle, vec2 center) {
   float s = sin(angle);
   float c = cos(angle);
   uv -= center;
   uv = mat2(c, -s, s, c) * uv;
   uv += center;
   return uv;
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir){
   vec3 lightDir = normalize(-light.direction);
   float diff = max(dot(normal, lightDir), 0.0);
   
   vec3 reflectDir = reflect(-lightDir, normal);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);

   vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, vs_out.TexCoords));
   vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, vs_out.TexCoords));
   vec3 specular = light.specular * spec * vec3(texture(texture_specular1, vs_out.TexCoords));
   
   return (ambient + diffuse + specular);
}

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

   //FragColor = vec4(vec3(texture(texture_diffuse1, TexCoords)), 1.0);

   vec2 tileCoord = floor(vs_out.fragPos.xz);
   float angle = hash(tileCoord) * 6.28318530718;
   vec2 uv = fract(vs_out.fragPos.xz);

   float scale = 0.5;
   uv = (uv - 0.5) * scale + 0.5;

   uv = rotateUV(uv, angle, vec2(0.5));
   vec4 color = texture(texture_diffuse1, uv);
   

   FragColor = color;
}