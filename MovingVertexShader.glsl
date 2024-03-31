#version 330 core
layout (location = 0) in vec3 aPos;
uniform vec4 VerticeTimeMultiplier;
void main()
{
   gl_Position = vec4(aPos.x, aPos.y * VerticeTimeMultiplier.y, aPos.z, 1.0);
}