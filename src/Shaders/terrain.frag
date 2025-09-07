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
uniform vec3 viewPos;

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

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec2 uv) {
   vec3 lightDir = normalize(-light.direction);

   // Ambient
   vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, uv));

   // Diffuse
   float diff = max(dot(normal, lightDir), 0.0);
   vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, uv));

   // Specular
   vec3 halfwayDir = normalize(lightDir + viewDir);
   float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
   
   vec3 specular = light.specular * spec * vec3(texture(texture_specular1, uv));

   return ambient + diffuse + specular;
}

void main(){

   vec2 tileCoord = floor(vs_out.fragPos.xz);
   float angle = hash(tileCoord) * 6.28318530718;
   vec2 uv = fract(vs_out.fragPos.xz);

   float scale = 0.5;
   uv = (uv - 0.5) * scale + 0.5;

   uv = rotateUV(uv, angle, vec2(0.5));
   vec3 normalizedNormal = normalize(vs_out.normal);
   vec3 viewDir = normalize(viewPos - vs_out.fragPos);
   vec3 result = calcDirLight(light, normalizedNormal, viewDir, uv);

   //vec3 result = vec3(texture(texture_specular1, uv));
   //FragColor = vec4(normalize(vs_out.normal) * 0.5 + 0.5, 1.0);
   
   FragColor = vec4(result, 1.0);
}