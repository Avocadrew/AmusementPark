#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glad/glad.h>
#include <GL/glu.h>
//************************************************************************
//
// * capture Pixelation by 360 camera
//
//========================================================================
class Pixelation
{
public:
	Pixelation() : size(256, 256)
	{
		GeneratingQUAD(256, 256);
	}

	Pixelation(int w, int h) : size(w, h)
	{
		GeneratingQUAD(w, h);
	}

	~Pixelation() {}
	const GLuint getID() const { return id; }
	const glm::vec2 getSize() const { return size; }

	const void bind(GLenum bind_unit)
	{
		glActiveTexture(GL_TEXTURE0 + bind_unit);
		glBindTexture(GL_TEXTURE_2D, id);
	}

private:
	GLuint id;

	void GeneratingQUAD(int width, int height)
	{
		//************************************************************************
		//
		// * generate a new texture
		//
		//========================================================================
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	glm::vec2 size;
};