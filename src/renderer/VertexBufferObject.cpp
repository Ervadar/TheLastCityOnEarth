#include "VertexBufferObject.h"

VertexBufferObject::VertexBufferObject()
{
	dataUploaded = false;
	buffer = 0;
}

void VertexBufferObject::createVBO(int size)
{
	glGenBuffers(1, &buffer);
	data.reserve(size);
	this->size = size;
	currentSize = 0;
}

void VertexBufferObject::deleteVBO()
{
	glDeleteBuffers(1, &buffer);
	dataUploaded = false;
	data.clear();
}

void VertexBufferObject::bindVBO(int bufferType)
{
	this->bufferType = bufferType;
	glBindBuffer(bufferType, buffer);
}

void VertexBufferObject::uploadDataToGPU(int drawingHint)
{
	glBufferData(bufferType, data.size(), &data[0], drawingHint);
	dataUploaded = true;
	data.clear();
}

void VertexBufferObject::addData(void* a_data, GLuint dataSize)
{
	data.insert(data.end(), (GLbyte*)a_data, (GLbyte*)a_data+dataSize);
	currentSize += dataSize;
}

void* VertexBufferObject::getDataPointer()
{
	if(dataUploaded) return nullptr;
	return (void*)data[0];
}

GLuint VertexBufferObject::getBufferID()
{
	return buffer;
}

int VertexBufferObject::getCurrentSize()
{
	return currentSize;
}