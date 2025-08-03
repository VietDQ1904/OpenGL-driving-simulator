#version 440 core
out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

in vec2 TexCoords;
in vec3 normal;
in vec3 fragPos;

// Generate a pseudo-random number.
float hash(vec2 p) {
   return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

vec2 rotateUV(vec2 uv, float angle, vec2 center) {
   float s = sin(angle);
   float c = cos(angle);
   uv -= center;
   uv = mat2(c, -s, s, c) * uv;
   uv += center;
   return uv;
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

   vec2 tileCoord = floor(fragPos.xz);
   float angle = hash(tileCoord) * 6.28318530718;
   vec2 uv = fract(fragPos.xz);

   float scale = 0.5;
   uv = (uv - 0.5) * scale + 0.5;

   uv = rotateUV(uv, angle, vec2(0.5));
   vec4 color = texture(texture_diffuse1, uv);

   FragColor = color;
}