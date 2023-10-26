#ifndef MESH_BUILDER_H
#define MESH_BUILDER_H

#include "Mesh.h"
#include "Vertex.h"

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
	static Mesh* GenerateCircle(const std::string& meshName, glm::vec3 color, float radius = 1.0f, int numSlice = 360);
	static Mesh* GenerateSphere(const std::string& meshName,glm::vec3 color,float radius = 1.f,int numSlice = 360,int numStack = 360);

};

#endif