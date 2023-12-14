#ifndef MESH_BUILDER_H
#define MESH_BUILDER_H

#include "Mesh.h"
#include "Vertex.h"
#include "LoadOBJ.h"

/******************************************************************************/
/*!
		Class MeshBuilder:
\brief	Provides methods to generate mesh of different shapes
*/
/******************************************************************************/
class MeshBuilder
{
public:
	static Mesh* GenerateAxes(const std::string &meshName, float lengthX, float lengthY, float lengthZ);

	static Mesh* GenerateQuad(const std::string& meshName, glm::vec3 color, float length = 1.f);

	//1.
	static Mesh* GenerateCylinder(const std::string& meshName, glm::vec3 color, float topRadius = 1, float btmRadius = 1, int height = 1, int numSlice = 360);
	//2.
	static Mesh* GenerateSphere(const std::string& meshName,glm::vec3 color,float radius = 1.f,int numSlice = 360,int numStack = 360);
	//3.
	static Mesh* GenerateTorus(const std::string& meshName, glm::vec3 color, float innerR = 1.f, float outerR = 1.f, int numSlice = 360, int numStack = 360);
	//4.
	static Mesh* GenerateCube(const std::string& meshName, glm::vec3 color, float topRadius = 1, float btmRadius = 1, int height = 1, int numSlice = 360);

	static Mesh* GenerateOBJ(const std::string& meshName, const std::string& file_path);


};

#endif