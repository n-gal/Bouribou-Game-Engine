#version 330 core
out vec4 FragColor;

struct material {
    vec3 diffuse;
    vec3 specular;
    float smoothness;
};

uniform vec3 viewPos;

uniform material inMaterial;


uniform vec3 lightColor;
uniform vec3 lightPos;
uniform float spreadStrength = 0.1;
uniform float ambientStrength = 0.1;
uniform float lightFalloff = 1;

uniform vec3 lightColor2;
uniform vec3 lightPos2;
uniform float spreadStrength2 = 0.1;
uniform float ambientStrength2 = 0.1;
uniform float lightFalloff2 = 1;

in vec3 Normal;
in vec3 FragPos;

void main()
{
    // ambient lighting
    vec3 ambient = ambientStrength * lightColor * inMaterial.diffuse;

    // spread lighting, omnidirectional lighting that ignores normals
    vec3 spread = spreadStrength * lightColor;

    // diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), inMaterial.smoothness);
    vec3 specular = inMaterial.specular * spec * lightColor;

    // light attenuation
    float lightDistance = distance(FragPos, lightPos);
    float attenuation = 1.0 / (1.0 + lightFalloff * lightDistance * lightDistance);

    // result
    vec3 result = ambient + ((((spread + diffuse + specular) * inMaterial.diffuse)) * attenuation);



    // light 2


    // ambient lighting
    ambient = ambientStrength2 * lightColor2 * inMaterial.diffuse;

    // spread lighting, omnidirectional lighting that ignores normals
    spread = spreadStrength2 * lightColor2;

    // diffuse lighting
    lightDir = normalize(lightPos2 - FragPos);
    diff = max(dot(norm, lightDir), 0.0);
    diffuse = diff * lightColor2;

    // specular lighting
    reflectDir = reflect(-lightDir, norm);
    spec = pow(max(dot(viewDir, reflectDir), 0.0), inMaterial.smoothness);
    specular = inMaterial.specular * spec * lightColor2;

    // light attenuation
    lightDistance = distance(FragPos, lightPos2);
    attenuation = 1.0 / (1.0 + lightFalloff2 * lightDistance * lightDistance);

    result = result + (ambient + ((((spread + diffuse + specular) * inMaterial.diffuse)) * attenuation));

    FragColor = vec4(result, 1.0);
}