#pragma once
#include "GL\glew.h"
#include <string>

class SSBO
{
public:
	SSBO(GLuint program, std::string name, size_t element_size);
	void Bind();
	void Read(size_t size, void* data);
	void Write(size_t size, const void* data);
	GLuint GetIdentifier();
	GLuint GetBinding();
private:
	static GLuint next_binding;
	GLuint identifier;
	GLuint binding;
	size_t element_size;
};

