#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;


uniform mat4 modelMatrix;
uniform mat4 view;
uniform mat4 projection;

//normals transformation matrix
uniform mat3 normalMatrix;

//transformation matrix to light POV
uniform mat4 lightPOV;

//direction of incoming light
uniform vec3 lightVector;

//direction of incoming light in view coordinates
out vec3 lightDir;
//normals in view coordinates
out vec3 vNormal;

out vec3 vViewPosition;
out vec4 posLightPOV;


void main()
{

  vec4 mPosition = modelMatrix * vec4( position, 1.0 );
  // vertex position in camera coordinates
  vec4 mViewPosition = view * mPosition;

  // view direction, negated to have vector from the vertex to the camera
  vViewPosition = -mViewPosition.xyz;

  vNormal = normalize( normalMatrix * normal );

  // light incidence directions in view coordinate
  lightDir = vec3(view  * vec4(lightVector, 0.0));

  gl_Position = projection * mViewPosition;

  // vertex position in "light coordinates"
  posLightPOV = lightPOV * mPosition;

}
