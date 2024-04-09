#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float blend = 0.5;
uniform float vertexColorStrength = 1;

void main()
{
    vec3 defaultColor = vec3(1.0); 
    vec3 blendedColor = mix(defaultColor, ourColor, vertexColorStrength);
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), blend) * vec4(blendedColor,1);
}