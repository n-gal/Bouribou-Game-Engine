#version 330 core
out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float specularStrength = 0.5;
uniform float smoothness = 32;
uniform float ambientStrength = 0.1;
uniform float lightFalloff = 1;

in vec3 Normal;
in vec3 FragPos;

void main()
{
    

    // ambient lighting
    vec3 ambient = ambientStrength * lightColor;

    // diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), smoothness);
    vec3 specular = specularStrength * spec * lightColor;

    float lightDistance = distance(FragPos, lightPos);

    // result
    vec3 result = ((ambient + diffuse + specular) * objectColor) / (lightDistance * 0.5);
    FragColor = vec4(result, 1.0);
}