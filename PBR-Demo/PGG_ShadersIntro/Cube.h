
#ifndef __CUBE_H__
#define __CUBE_H__

// This is the main SDL include file
#include <SDL/SDL.h>
#include "glew.h"

class Cube
{
public:
	Cube();
	~Cube();

	void Draw();

protected:
	GLuint _VAO;
	unsigned int _numVertices;
};

#endif
