#version 330 core
out vec4 FragColor;

#define MAX_LIGHTS 100

struct material {
    vec3 diffuse;
    vec3 specular;
    float smoothness;
    float reflectivity;
};

struct light {
    vec3 lightColor;
    vec3 lightPos;
    float spreadStrength;
    float ambientStrength;
    float lightFalloff;
};

in vec3 Normal;
in vec3 Position;

uniform light[MAX_LIGHTS] inLight;
uniform material inMaterial;

uniform vec3 viewPos;
uniform samplerCube skybox;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 result = vec3(0, 0, 0);
    int numLights = MAX_LIGHTS;
    vec3 I = normalize(Position - viewPos);
    vec3 R = reflect(I, normalize(Normal));


    for (int i = 0; i < numLights; ++i)
    {
        // ambient lighting
        vec3 ambient = inLight[i].ambientStrength * inLight[i].lightColor * inMaterial.diffuse;

        // spread lighting, omnidirectional lighting that ignores normals
        vec3 spread = inLight[i].spreadStrength * inLight[i].lightColor;

        // diffuse lighting
        vec3 lightDir = normalize(inLight[i].lightPos - Position);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * inLight[i].lightColor;

        // specular lighting
        vec3 viewDir = normalize(viewPos - Position);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), inMaterial.smoothness);
        vec3 specular = inMaterial.specular * spec * inLight[i].lightColor;

        // light attenuation
        float lightDistance = distance(Position, inLight[i].lightPos);
        float attenuation = 1.0 / (1.0 + inLight[i].lightFalloff * lightDistance * lightDistance);

        // result
        result = result + (ambient + ((((spread + diffuse + specular) * inMaterial.diffuse)) * attenuation));
    }
    
    vec4 reflectiveResult = vec4(texture(skybox, R).rgb, 1.0);

    vec3 mixedReflect = mix(inMaterial.diffuse, texture(skybox, R).rgb, inMaterial.reflectivity);

    FragColor = vec4(result,0) * (vec4(mixedReflect,1));
}