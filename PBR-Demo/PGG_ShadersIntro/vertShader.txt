#version 430 core
// This is the vertex shader
// The program in this file will be run separately for each vertex in the model

// This is the per-vertex input
layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 vNormalIn;

// These variables will be the same for every vertex in the model
uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

uniform vec4 worldSpaceLightPos = {1,0.0,1,1};

// These are the outputs from the vertex shader
// The data will (eventually) end up in the fragment shader
out vec3 eyeSpaceNormalV;
out vec3 eyeSpaceLightPosV;
out vec3 eyeSpaceVertPosV;

// The actual program, which will run on the graphics card
void main()
{
	// Viewing transformation
	// Incoming vertex position is multiplied by: modelling matrix, then viewing matrix, then projection matrix
	// gl_position is a special output variable
	gl_Position = projMat * viewMat * modelMat * vPosition;

	// These two variables will be useful for our lighting calculations in the fragment shader
	// This is the vertex position in eye space, we get it by multiplying the object-space vertex position (input) by the model and view matrices
	eyeSpaceVertPosV = vec3(viewMat * modelMat * vPosition);
	// This is the light's position in eye space
	// The light starts in world space so we only need to multiply it by the viewing matrix
	eyeSpaceLightPosV = vec3(viewMat * worldSpaceLightPos);
	
	// The surface normal is multiplied by the model and viewing matrices
	// This doesn't need to 'move' so we cast down to a 3x3 matrix
	eyeSpaceNormalV = mat3(viewMat * modelMat) * vNormalIn;
}
