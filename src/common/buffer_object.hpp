#pragma once

#include "utils.hpp"
#include "gl.hpp"

class BufferObject: public NonCopyable
{
public:
	BufferObject()
	{
		glGenBuffers(1, &bo);
	}

	BufferObject(GLuint bo):
		bo(bo)
	{}

	BufferObject(BufferObject&& other)
	{
		*this = std::move(other);
	}

	BufferObject& operator=(BufferObject&& other)
	{
		bo = other.bo;
		other.bo = 0;
		return *this;
	}

	~BufferObject()
	{
		glDeleteBuffers(1, &bo);
	}

	operator GLuint() const
	{
		return bo;
	}

private:
	GLuint bo;
};
