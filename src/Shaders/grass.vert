#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
   vec2 TexCoords;
   vec3 FragPos;
   mat3 TBN;
} vs_out;

uniform float time;                 
uniform vec3 windDirection;
uniform float windStrength; 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

vec3 cubicBezier(vec3 p0, vec3 p1, vec3 p2, vec3 p3, float t)
{
   float u = 1.0 - t;
   float tt = t * t;
   float uu = u * u;
   float uuu = uu * u;
   float ttt = tt * t;

   return uuu * p0 +
         3.0 * uu * t * p1 +
         3.0 * u * tt * p2 +
         ttt * p3;
}


void main()
{
   mat3 normalMatrix = transpose(inverse(mat3(model)));

   vec3 T = normalize(normalMatrix * aTangent);
   vec3 N = normalize(normalMatrix * aNormal);
   T = normalize(T - dot(T, N) * N);
   vec3 B = cross(N, T);

   vs_out.TBN = mat3(T, B, N);
   vs_out.TexCoords = aTexCoords;

   float sway = sin(time + aPos.x * 2.0) * 0.5 + 0.5;
   float factor = smoothstep(0.0, 1.0, aPos.y);
   
   // Calculate control points of Bezier curve.
   vec3 p0 = aPos;
   vec3 p1 = aPos + N * 0.1;
   vec3 p2 = aPos + windDirection * windStrength * sway * factor;
   vec3 p3 = aPos + windDirection * windStrength * 2.0 * sway * factor;

   vec3 displacedPos = cubicBezier(p0, p1, p2, p3, factor);

   // New position.
   vs_out.FragPos = vec3(model * vec4(displacedPos, 1.0));

   gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}