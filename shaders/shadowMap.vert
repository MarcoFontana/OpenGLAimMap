#version 410 core

layout (location = 0) in vec3 position;

uniform mat4 lightPOV;

uniform mat4 modelMatrix;

void main()
{
	//vertex pos from light POV
    gl_Position = lightPOV * modelMatrix * vec4(position, 1.0f);
}
