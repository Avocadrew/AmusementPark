#include "Utilities/Pnt3f.H"
#include "Utilities/3DUtils.h"
#include <string>
#include <vector>
#include <fstream>
#include <GL/gl.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/matrix.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

/*
 * from OpenGL-tutorial
 * url: https://github.com/opengl-tutorials/ogl/blob/master/common/objloader.cpp
 */

class Texture
{
public:
	Texture();
	~Texture();

	int width;
	int height;

};

class Object
{
public:
	Object();
	Object(const std::string);

	bool loadOBJ(const std::string fileName);

	/*void draw(bool doingShadows);

	void translate(const Pnt3f move);

	void scaling(const Pnt3f scale);

	void rotate(const Pnt3f rotate);*/

	~Object();

	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> vertexIndices;
	float* vecticesForShader;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	float* normalsForShader;
	
	Pnt3f pos;

	Texture * dotCubeTexture;

private:

};