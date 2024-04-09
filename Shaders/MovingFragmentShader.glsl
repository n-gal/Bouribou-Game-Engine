#version 330 core
uniform vec4 NewColor;
out vec4 FragColor;
void main()
{
   FragColor = NewColor;
}