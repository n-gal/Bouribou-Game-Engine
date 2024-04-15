#version 330 core
out vec4 FragColor;

#define MAX_LIGHTS 100

struct material {
    sampler2D diffuse;
    sampler2D specular;
    float smoothness;
};

struct light {
    vec3 lightColor;
    vec3 lightPos;
    float spreadStrength;
    float ambientStrength;
    float lightFalloff;
};


uniform light[MAX_LIGHTS] inLight;

uniform vec3 viewPos;

uniform material inMaterial;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

void main()
{
    int numLights = MAX_LIGHTS;

    vec3 result = vec3(0,0,0);
    vec3 norm = normalize(Normal);

    vec3 diffuseTexture = vec3(texture(inMaterial.diffuse, TexCoord));
    vec3 specularTexture = vec3(texture(inMaterial.specular, TexCoord));

    for (int i = 0; i < numLights; ++i)
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
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), inMaterial.smoothness);
        vec3 specular = specularTexture * (spec*3) * inLight[i].lightColor;

        // light attenuation
        float lightDistance = distance(FragPos, inLight[i].lightPos);
        float attenuation = 1.0 / (1.0 + inLight[i].lightFalloff * lightDistance * lightDistance);

        // result
        result = result + (ambient + ((((spread + diffuse + specular) * diffuseTexture)) * attenuation));
    }

    FragColor = vec4(result, 1.0);
}