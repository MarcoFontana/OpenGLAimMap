#version 410 core

layout(location = 0) in vec3 vertices;
layout(location = 1) in vec4 xyzs; 
layout(location = 2) in vec4 color; 

out vec4 particlecolor;

uniform vec3 cameraRightVector;
uniform vec3 cameraUpVector;
uniform mat4 ViewProjection;

void main()
{
	float size = xyzs.w;
	vec3 center = xyzs.xyz;
	
	vec3 worldPosition = 
		center
		+ cameraRightVector * vertices.x * size
		+ cameraUpVector * vertices.y * size;

	// vertex pos with particle rotated to face the camera
	gl_Position = ViewProjection * vec4(worldPosition, 1.0f);

	particlecolor = color;
}