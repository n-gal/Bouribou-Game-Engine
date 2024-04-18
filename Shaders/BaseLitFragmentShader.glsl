#version 330 core
out vec4 FragColor;

#define MAX_LIGHTS 3

struct material {
    sampler2D diffuse;
    sampler2D specular;
    float roughness;
};

struct light {
    vec3 lightColor;
    vec3 lightPos;
    float spreadStrength;
    float ambientStrength;
    float lightFalloff;
    int type;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    float intensity;
};


uniform light[MAX_LIGHTS] inLight;

uniform vec3 viewPos;

uniform material inMaterial;

uniform samplerCube skybox;



in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

float fresnelSchlick(float cosTheta, float refIndex);

vec3 calculatePointLights(vec3 diffuseTexture, vec3 viewDir, float fres, float specularStrength, vec3 specularTexture, vec3 norm, float smoothness, int i);
vec3 calculateSpotLights(vec3 diffuseTexture, vec3 viewDir, float fres, float specularStrength, vec3 specularTexture, vec3 norm, float smoothness, int i);
vec3 calculateDirectionalLights(vec3 diffuseTexture, vec3 viewDir, float fres, float specularStrength, vec3 specularTexture, vec3 norm, float smoothness, int i);

void main()
{
    float smoothness = 1 / inMaterial.roughness * 100;
    smoothness = clamp(smoothness, 0.1, 500);
    vec3 result = vec3(0, 0, 0);
    vec3 norm = normalize(Normal);


    vec3 diffuseTexture = vec3(texture(inMaterial.diffuse, TexCoord));
    vec3 specularTexture = vec3(texture(inMaterial.specular, TexCoord));
    vec3 viewDir = normalize(viewPos - FragPos);

    float cosTheta = dot(viewDir, normalize(Normal));
    float fres = fresnelSchlick(cosTheta, 1);
    float specularStrength = clamp(10 - (inMaterial.roughness), 0, 10);

    // cubemapped reflections
    vec3 I = normalize(FragPos - viewPos);
    vec3 R = reflect(I, normalize(Normal));
    vec4 reflectiveResult = (vec4(textureLod(skybox, R, inMaterial.roughness * 1.5).rgb, 1.0) * vec4(specularTexture, 1)) * fres;
    reflectiveResult = clamp(reflectiveResult - (inMaterial.roughness * 0.05), 0, 1);

    for (int i = 0; i < MAX_LIGHTS; ++i)
    {
        if(inLight[i].type == 0)
            result = result + calculatePointLights(diffuseTexture, viewDir, fres, specularStrength, specularTexture, norm, smoothness, i);
        if (inLight[i].type == 1)
            result = result + calculateDirectionalLights(diffuseTexture, viewDir, fres, specularStrength, specularTexture, norm, smoothness, i);
        if (inLight[i].type == 2)
            result = result + calculateSpotLights(diffuseTexture, viewDir, fres, specularStrength, specularTexture, norm, smoothness, i);
    }
    FragColor = vec4(result, 1.0) + reflectiveResult;
}

float fresnelSchlick(float cosTheta, float refIndex)
{
    float r0 = (1.0 - refIndex) / (1.0 + refIndex);
    r0 = r0 * r0;
    r0 = r0 + (1.0 - r0) * pow(1.0 - cosTheta, 5.0);
    return clamp(r0, 0, 1);
}

vec3 calculatePointLights(vec3 diffuseTexture, vec3 viewDir, float fres, float specularStrength, vec3 specularTexture, vec3 norm, float smoothness, int i)
{
    // ambient lighting
    vec3 ambient = inLight[i].ambientStrength * inLight[i].lightColor * diffuseTexture;

    // spread lighting, omnidirectional lighting that ignores normals
    vec3 spread = inLight[i].spreadStrength * inLight[i].lightColor;

    // diffuse lighting
    vec3 lightDir = normalize(inLight[i].lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * inLight[i].lightColor;

    // specular lighting
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), smoothness);
    vec3 specular = specularTexture * (spec * specularStrength) * inLight[i].lightColor;

    // fresnel term in specular lighting, physically based but less cool lookng :(
    specular = specular * fres;

    // light attenuation
    float lightDistance = distance(FragPos, inLight[i].lightPos);
    float attenuation = 1.0 / (1.0 + inLight[i].lightFalloff * lightDistance * lightDistance);

    // result
    vec3 result = (ambient + (((((spread + diffuse + specular) * inLight[i].intensity) * diffuseTexture)) * attenuation));
    return result;
}

vec3 calculateDirectionalLights(vec3 diffuseTexture, vec3 viewDir, float fres, float specularStrength, vec3 specularTexture, vec3 norm, float smoothness, int i)
{
    // ambient lighting
    vec3 ambient = inLight[i].ambientStrength * inLight[i].lightColor * diffuseTexture;

    // diffuse lighting
    vec3 lightDir = normalize(-inLight[i].direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * inLight[i].lightColor;

    // specular lighting
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), smoothness);
    vec3 specular = specularTexture * (spec * specularStrength) * inLight[i].lightColor;

    // fresnel term in specular lighting, physically based but less cool lookng :(
    specular = specular * fres;


    // result
    vec3 result = ambient + (((diffuse + specular) * inLight[i].intensity) * diffuseTexture);
    return result;
}

vec3 calculateSpotLights(vec3 diffuseTexture, vec3 viewDir, float fres, float specularStrength, vec3 specularTexture, vec3 norm, float smoothness, int i)
{
    vec3 lightDir = normalize(inLight[i].lightPos - FragPos);
    float theta = dot(lightDir, normalize(-inLight[i].direction));
    float epsilon = inLight[i].cutOff - inLight[i].outerCutOff;
    float intensity = clamp((theta - inLight[i].outerCutOff) / epsilon, 0.0, 1.0);

    vec3 result = vec3(0, 0, 0);

    // ambient lighting
    vec3 ambient = inLight[i].ambientStrength * inLight[i].lightColor * diffuseTexture;

    // spread lighting, omnidirectional lighting that ignores normals
    vec3 spread = inLight[i].spreadStrength * inLight[i].lightColor;

    // diffuse lighting
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * inLight[i].lightColor;

    // specular lighting
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), smoothness);
    vec3 specular = specularTexture * (spec * specularStrength) * inLight[i].lightColor;

    // fresnel term in specular lighting, physically based but less cool lookng :(
    specular = specular * fres;

    // light attenuation
    float lightDistance = distance(FragPos, inLight[i].lightPos);
    float attenuation = 1.0 / (1.0 + inLight[i].lightFalloff * lightDistance * lightDistance);

    diffuse *= intensity;
    specular *= intensity;

    // result
    result = (ambient + (((((spread + diffuse + specular) * inLight[i].intensity) * diffuseTexture)) * attenuation));
    return result;
}