
#include "Cube.h"

// The GLM library contains vector and matrix functions and classes for us to use
// They are designed to easily work with OpenGL!
#include <GLM/glm.hpp> // This is the main GLM header
#include <GLM/gtc/matrix_transform.hpp> // This one lets us use matrix transformations
#include <GLM/gtc/type_ptr.hpp> // This one gives us access to a utility function which makes sending data to OpenGL nice and easy



class Scene
{
public:

	Scene();
	~Scene();

	void ChangeCameraAngleX( float value ) { _cameraAngleX += value; }
	void ChangeCameraAngleY( float value ) { _cameraAngleY += value; }

	void Update( float deltaTs );


	void Draw();


	// Getters and Setters for various scene object variables
	// These are very specific to the scene, your design should move them out of here
	void SetCubeDiffuseColour(glm::vec3 value) { _cubeDiffuseColour = value; }
	glm::vec3 GetCubeDiffuseColour() { return _cubeDiffuseColour; }

	void SetAnimateLight(bool value) { _animateLight = value; }
	bool GetAnimateLight() { return _animateLight; }

	void SetLightAngle(float value) { _cube2Angle = value; }
	float GetLightAngle() { return _cube2Angle; }

	void SetAnimateCube(bool value) { _animateCentreCube = value; }
	bool GetAnimateCube() { return _animateCentreCube; }

	void SetCubeAngle(float value) { _cube1Angle = value; }
	float GetCubeAngle() { return _cube1Angle; }

	void SetCubemetallic(float value) { _shaderMetallicLocation = value; }
	float GetCubemetallic() { return _shaderMetallicLocation; }

	void SetcubeRoughness(float value) { _shaderRoughnessLocation = value; }
	float GetcubeRoughness() { return _shaderRoughnessLocation; }


protected:

	Cube _cubeModel;

	// Model matrices for three cubes
	glm::mat4 _modelMatrixCube1;
	glm::mat4 _modelMatrixCube2;
	glm::mat4 _modelMatrixCube3;
		
	// All cubes share the same viewing matrix - this defines the camera's orientation and position
	glm::mat4 _viewMatrix;
	
	// The projection matrix defines the camera's view (like its lens)
	glm::mat4 _projMatrix;


	// Angle of rotation for our cube
	float _cube1Angle;
	float _cube2Angle;
	float _cameraAngleX, _cameraAngleY;


	// TODO: Exercise: extract all shader code and put it in its own class

	GLuint _shaderProgram;

	// These are for storing the Uniform locations of shader variables
	// We need these so we can send Uniform data to them
	int _shaderModelMatLocation;
	int _shaderViewMatLocation;
	int _shaderProjMatLocation;

	int _shaderMetallicLocation;
	int _shaderDiffuseColLocation, _shaderEmissiveColLocation;
	int _shaderWSLightPosLocation;
	int _shaderRoughnessLocation;

	// Utility functions to help us with building our shaders
	bool CheckShaderCompiled( GLint shader );
	void BuildShaders();

	glm::vec3 _cubeDiffuseColour;

	// Bools for turning animations on and off 
	bool _animateCentreCube, _animateLight;

};
