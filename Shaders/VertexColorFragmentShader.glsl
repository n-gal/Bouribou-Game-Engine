#version 330 core
in vec4 VertColour;
out vec4 FragColor;
void main()
{
   FragColor = vec4(VertColour.x, VertColour.y, VertColour.z, 1.0f);
}