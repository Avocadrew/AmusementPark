#include "Object.H"

Object::
Object()
{

}

Object::
Object(const std::string fileName)
{
    loadOBJ(fileName);
}

// from opengl-tutorial
bool Object::
loadOBJ(const std::string fileName)
{
	printf("Loading OBJ file %s...\n", fileName.c_str());

	std::vector<unsigned int> uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE* file = fopen(fileName.c_str(), "r");
	if (file == NULL) {
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				fclose(file);
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		vertices.push_back(vertex);
		uvs.push_back(uv);
		normals.push_back(normal);
	}
	vertices = temp_vertices;
	/*vecticesForShader = new float[vertices.size() * 3];
	normalsForShader = new float[normals.size() * 3];
	for (int i = 0; i < vertices.size(); i++)
	{
		vecticesForShader[i * 3] = vertices[i].x;
		vecticesForShader[i * 3 + 1] = vertices[i].y;
		vecticesForShader[i * 3 + 2] = vertices[i].z;

		normalsForShader[i * 3] = normals[i].x;
		normalsForShader[i * 3 + 1] = normals[i].y;
		normalsForShader[i * 3 + 2] = normals[i].z;
	}*/

	fclose(file);
	return true;
}


//void Object::
//draw(bool doingShadows = false)
//{
//	glTexImage2D(GL_TEXTURE_2D, 0, 3, texture->width, texture->height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture->pix);
//	glBegin(GL_TRIANGLES);
//    if (!doingShadows)
//        glColor3ub(255, 255, 255);
//    for (unsigned int i = 0; i < vertices.size(); i++)
//    {
//        glNormal3f(normals[i].x, normals[i].y, normals[i].z);
//        glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
//    }
//    glEnd();
//}
//
//void Object::
//translate(const Pnt3f _move)
//{
//	glm::vec3 move(_move.x, _move.y, _move.z);
//    for (int i = 0; i < vertices.size(); ++i)
//        vertices[i] = vertices[i] + move;
//
//    pos = pos + _move;
//}
//
//void Object::
//scaling(const Pnt3f scale)
//{
//    for (int i = 0; i < vertices.size(); ++i)
//    {
//        vertices[i].x *= scale.x;
//        vertices[i].y *= scale.y;
//        vertices[i].z *= scale.z;
//    }
//}
//
//void Object::
//rotate(const Pnt3f rotate)
//{
//    glm::vec3 eulerAngles(degreesToRadians(rotate.x), degreesToRadians(rotate.y), degreesToRadians(rotate.z));
//    glm::quat quaternion = glm::quat(eulerAngles);
//    glm::mat4 rotationMatrix = glm::toMat4(quaternion);
//	glm::vec4 vertex, normal;
//    for (int i = 0; i < vertices.size(); ++i)
//    {
//		vertex = glm::vec4(vertices[i], 1);
//		vertices[i] = rotationMatrix * vertex;
//		normal = glm::vec4(normals[i], 1);
//		normals[i] = rotationMatrix * normal;
//    }
//}

Object::
~Object()
{

}