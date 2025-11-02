// vertex.glsl
#version 150

in vec4 vertex;
in vec3 normal;
in vec4 diffuse;
in vec4 colour;

uniform mat4 worldViewProj;

out vec4 fragColour;

void main()
{
    gl_Position = worldViewProj * vertex;
    //fragColour = vec4(0.2, 0.4, 0.8, 1.0); // 强制水蓝色
	fragColour = colour;
}