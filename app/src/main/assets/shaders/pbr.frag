#version 120
#extension GL_OES_standard_derivatives : enable
precision mediump float;

//layout(location = 0) out vec4 frag_color;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

// Lights
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

varying vec3 Normal;
varying vec3 WorldPos;
varying vec2 TexCoords;

const float PI = 3.14159265359f;

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture2D(normalMap, TexCoords).xyz * 2.0 - 1.0;

    //vec3 Q1 = dFdx(WorldPos);
    vec3 Q1 =  vec3(dFdx(WorldPos.x),dFdx(WorldPos.y),dFdx(WorldPos.z));
    //vec3 Q2 = dFdy(WorldPos);
    vec3 Q2 = vec3(dFdy(WorldPos.x),dFdy(WorldPos.y),dFdy(WorldPos.z));
    //vec2 st1 = dFdx(TexCoords);
    vec2 st1 = vec2(dFdx(TexCoords.x), dFdx(TexCoords.y));
    //vec2 st2 = dFdy(TexCoords);
    vec2 st2 = vec2(dFdy(TexCoords.x), dFdy(TexCoords.y));

    vec3 N = normalize(Normal);
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    //return normalize(TBN * tangentNormal);
    return normalize(Normal);
}

float DistributionGGX(float NdotH, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float denom = NdotH * NdotH * (a2 - 1.0f) + 1.0f;
    denom = PI * denom * denom;

    return a2 / max(denom, 0.0000001f);
}

float GeometrySmith(float NdotV, float NdotL, float roughness)
{
    float r = roughness + 1.0f;
    float k = (r * r) / 8.0f;
    float ggx1 = NdotV / (NdotV * (1.0f - k) + k);
    float ggx2 = NdotL / (NdotL * (1.0f - k) + k);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float HdotV, vec3 baseReflectivity)
{
    return baseReflectivity + (1.0f - baseReflectivity) * pow(1.0f - HdotV, 5.0f);
}

void main()
{
    vec3 albedo = texture2D(albedoMap, TexCoords).rgb;
    float metallic = texture2D(metallicMap, TexCoords).r;
    float roughness = texture2D(roughnessMap, TexCoords).r;
    float ao = texture2D(aoMap, TexCoords).r;

    vec3 N = getNormalFromMap();
    vec3 V = normalize(camPos - WorldPos);

    // Calculate reflectence
    vec3 baseReflectivity = mix(vec3(0.04f), albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);

    // Calculate per-light radiance
    vec3 L = normalize(lightPos - WorldPos);
    vec3 H = normalize(V + L);
    float distance = length(lightPos - WorldPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = lightColor * attenuation;

    // Cook-Torrance BRDF
    float NdotV = max(dot(N, V), 0.0000001f);
    float NdotL = max(dot(N, L), 0.0000001f);
    float HdotV = max(dot(H, V), 0.0f);
    float NdotH = max(dot(N, H), 0.0f);

    float D = DistributionGGX(NdotH, roughness);
    float G = GeometrySmith(NdotV, NdotL, roughness);
    vec3 F = fresnelSchlick(HdotV, baseReflectivity);

    vec3 specular = D * G * F;
    specular /= 4.0f * NdotV * NdotL;

    // Energy Conservation
    vec3 kD = vec3(1.0) - F;
    kD *= 1.0f - metallic;

    Lo += (kD * albedo / PI + specular) * radiance * NdotL;

    // Ambient Lighting - Need to replace with environment lighting
    vec3 ambient = vec3(0.7712237862f * 0.61803398875f) * albedo;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0f));
    // gamma correct
    color = pow(color, vec3(1.0f / 2.2f));

    gl_FragColor = vec4(color, 1.0f);
}