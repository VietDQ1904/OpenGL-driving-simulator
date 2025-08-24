#version 440 core
out vec4 FragColor;

in VS_OUT {
   vec2 TexCoords;
   vec3 FragPos;
   mat3 TBN;
} fs_in;

struct DirLight {
   vec3 direction;
   vec3 color;
};

uniform DirLight light;
uniform vec3 viewPos;

uniform sampler2D texture_diffuse1;   
uniform sampler2D texture_normal1;    
uniform sampler2D texture_roughness1;
uniform sampler2D texture_metallic1;  

vec3 getNormalFromMap(){
   vec3 tangentNormal = texture(texture_normal1, fs_in.TexCoords).xyz * 2.0 - 1.0;
   return normalize(fs_in.TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness){
   float a      = roughness * roughness;
   float a2     = a * a;
   float NdotH  = max(dot(N, H), 0.0);
   float NdotH2 = NdotH * NdotH;

   float num   = a2;
   float denom = (NdotH2 * (a2 - 1.0) + 1.0);
   denom = 3.14159265 * denom * denom;

   return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness){
   float r = (roughness + 1.0);
   float k = (r * r) / 8.0;

   float num   = NdotV;
   float denom = NdotV * (1.0 - k) + k;

   return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness){
   float NdotV = max(dot(N, V), 0.0);
   float NdotL = max(dot(N, L), 0.0);
   float ggx1 = GeometrySchlickGGX(NdotV, roughness);
   float ggx2 = GeometrySchlickGGX(NdotL, roughness);
   return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0){
   return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main(){

   vec3 albedo     = pow(texture(texture_diffuse1, fs_in.TexCoords).rgb, vec3(2.2)); // gamma correct
   float roughness = texture(texture_roughness1, fs_in.TexCoords).r;
   float metallic  = texture(texture_metallic1, fs_in.TexCoords).r; // optional
   
   vec3 N = getNormalFromMap();
   vec3 V = normalize(viewPos - fs_in.FragPos);
   vec3 L = normalize(-light.direction);

   float cosTheta = dot(N, L);
   float lightAngle = acos(cosTheta);
   float intensity = 1.0 - smoothstep(30.0, 120.0, lightAngle);

   vec3 H = normalize(V + L);

   // F0 blend between dielectric and metallic
   vec3 F0 = vec3(0.04);
   F0 = mix(F0, albedo, metallic);

   // Cook-Torrance BRDF
   float NDF = DistributionGGX(N, H, roughness);
   float G   = GeometrySmith(N, V, L, roughness);
   vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

   vec3 nominator    = NDF * G * F;
   float denom       = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
   vec3 specular     = nominator / denom;

   vec3 kS = F;
   vec3 kD = vec3(1.0) - kS;
   kD *= 1.0 - metallic;

   float NdotL = max(dot(N, L), 0.0);
   vec3 Lo = (kD * albedo / 3.14159265 + specular) * light.color * NdotL;

   // Ambient
   vec3 ambient = 0.05 * intensity * albedo;

   vec3 color = ambient + Lo;
   // HDR tonemapping
   color = color / (color + vec3(1.0));
   // Gamma correct
   color = pow(color, vec3(1.0 / 2.2));

   FragColor = vec4(color, 1.0);
}