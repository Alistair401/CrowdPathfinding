#include "stdafx.h"
#include "SSBO.h"

GLuint SSBO::next_binding = 1;

SSBO::SSBO(GLuint program, std::string name, size_t element_size)
{
	GLuint index = glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, name.c_str());
	glGenBuffers(1, &identifier);
	binding = next_binding++;
	glShaderStorageBlockBinding(program, index, binding);
	this->element_size = element_size;
}

void SSBO::Bind()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, identifier);
}

void SSBO::Read(size_t size, void * data)
{
	Bind();
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size * element_size, data);
}

void SSBO::Write(size_t size, const void* data)
{
	Bind();
	glBufferData(GL_SHADER_STORAGE_BUFFER, size * element_size, data, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, identifier);
}

GLuint SSBO::GetBinding()
{
	return binding;
}

GLuint SSBO::GetIdentifier()
{
	return identifier;
}



