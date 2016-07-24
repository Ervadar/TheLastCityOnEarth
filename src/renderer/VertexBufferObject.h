#ifndef _VERTEXBUFFEROBJECT_H_
#define _VERTEXBUFFEROBJECT_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

/*
Class wrapping OpenGL's VBOs
*/
class VertexBufferObject
{
public:
	void createVBO(int size = 0);
	void deleteVBO();

	void bindVBO(int bufferType = GL_ARRAY_BUFFER);
	void uploadDataToGPU(int usageHint);
	
	void addData(void* a_data, GLuint dataSize);

	void* getDataPointer();
	GLuint getBufferID();

	int getCurrentSize();

	VertexBufferObject();

private:
	GLuint buffer;
	int size;
	int currentSize;
	int bufferType;
	std::vector<GLbyte> data;

	bool dataUploaded;
};

#endif
