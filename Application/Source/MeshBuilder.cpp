#include "MeshBuilder.h"
#include <GL\glew.h>
#include <vector>
#include <glm\gtc\constants.hpp>


/******************************************************************************/
/*!
\brief
Generate the vertices of a reference Axes; Use red for x-axis, green for y-axis, blue for z-axis
Then generate the VBO/IBO and store them in Mesh object

\param meshName - name of mesh
\param lengthX - x-axis should start at -lengthX / 2 and end at lengthX / 2
\param lengthY - y-axis should start at -lengthY / 2 and end at lengthY / 2
\param lengthZ - z-axis should start at -lengthZ / 2 and end at lengthZ / 2

\return Pointer to mesh storing VBO/IBO of reference axes
*/
/******************************************************************************/
Mesh* MeshBuilder::GenerateAxes(const std::string &meshName, float lengthX, float lengthY, float lengthZ)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	//x-axis
	v.pos = glm::vec3(-lengthX, 0, 0); v.color = glm::vec3(1, 0, 0);
	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(lengthX, 0, 0); v.color = glm::vec3(1, 0, 0);
	vertex_buffer_data.push_back(v);

	//y-axis
	v.pos = glm::vec3(0, -lengthY, 0); v.color = glm::vec3(0, 1, 0);
	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(0, lengthY, 0); v.color = glm::vec3(0, 1, 0);
	vertex_buffer_data.push_back(v);

	//z-axis
	v.pos = glm::vec3(0, 0, -lengthZ); v.color = glm::vec3(0, 0, 1);
	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(0, 0, lengthZ); v.color = glm::vec3(0, 0, 1);
	vertex_buffer_data.push_back(v);


	////x-axis
	//v.pos.Set(-lengthX, 0, 0);	v.color.Set(1, 0, 0);	vertex_buffer_data.push_back(v);
	//v.pos.Set(lengthX, 0, 0);	v.color.Set(1, 0, 0);	vertex_buffer_data.push_back(v);
	////y-axis
	//v.pos.Set(0, -lengthY, 0);	v.color.Set(0, 1, 0);	vertex_buffer_data.push_back(v);
	//v.pos.Set(0, lengthY, 0);	v.color.Set(0, 1, 0);	vertex_buffer_data.push_back(v);
	////z-axis
	//v.pos.Set(0, 0, -lengthZ);	v.color.Set(0, 0, 1);	vertex_buffer_data.push_back(v);
	//v.pos.Set(0, 0, lengthZ);	v.color.Set(0, 0, 1);	vertex_buffer_data.push_back(v);

	index_buffer_data.push_back(0);
	index_buffer_data.push_back(1);
	index_buffer_data.push_back(2);
	index_buffer_data.push_back(3);
	index_buffer_data.push_back(4);
	index_buffer_data.push_back(5);

	Mesh *mesh = new Mesh(meshName);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_LINES;

	return mesh;
}

/******************************************************************************/
/*!
\brief
Generate the vertices of a quad; Use random color for each vertex
Then generate the VBO/IBO and store them in Mesh object

\param meshName - name of mesh
\param lengthX - width of quad
\param lengthY - height of quad

\return Pointer to mesh storing VBO/IBO of quad
*/
/******************************************************************************/
Mesh* MeshBuilder::GenerateQuad(const std::string &meshName, glm::vec3 color, float length)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	v.color = color;

	// Add the vertices here
	v.pos = glm::vec3(0.5f * length, -0.5f * length, -0.5f);  vertex_buffer_data.push_back(v); //V0
	v.pos = glm::vec3(0.5f * length, 0.5f * length, -0.5f);   vertex_buffer_data.push_back(v); //V1
	v.pos = glm::vec3(-0.5f * length, 0.5f * length, -0.5f);  vertex_buffer_data.push_back(v); //V2
	v.pos = glm::vec3(-0.5f * length, -0.5f * length, -0.5f); vertex_buffer_data.push_back(v); //V3

	v.pos = glm::vec3(0.5f * length, -0.5f * length, 0.5f * length);  vertex_buffer_data.push_back(v); //V4
	v.pos = glm::vec3(0.5f * length, 0.5f * length, 0.5f * length);  vertex_buffer_data.push_back(v);	//V5
	v.pos = glm::vec3(-0.5f * length, 0.5f * length, 0.5f * length);  vertex_buffer_data.push_back(v);	//V6
	v.pos = glm::vec3(-0.5f * length, -0.5f * length, 0.5f * length);  vertex_buffer_data.push_back(v); //V7

	index_buffer_data.push_back(0);
	index_buffer_data.push_back(1);
	index_buffer_data.push_back(2);

	index_buffer_data.push_back(0);
	index_buffer_data.push_back(2);
	index_buffer_data.push_back(3);

	index_buffer_data.push_back(1);
	index_buffer_data.push_back(5);
	index_buffer_data.push_back(4);

	index_buffer_data.push_back(1);
	index_buffer_data.push_back(4);
	index_buffer_data.push_back(0);

	// Create the new mesh
	Mesh* mesh = new Mesh(meshName);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLES;

	return mesh;
}

Mesh* MeshBuilder::GenerateCircle(const std::string& meshName, glm::vec3 color, float radius, int numSlice)
{
	Vertex v;                              // Vertex definition
	std::vector<Vertex> vertex_buffer_data;// Vertex Buffer Object ( VBOs )
	std::vector<GLuint> index_buffer_data; // Element Buffer Object ( EBOs )

	v.color = color;

	float anglePerSlice = glm::two_pi<float>() / numSlice;

	v.pos = glm::vec3(0, 0, 0);
	vertex_buffer_data.push_back(v);

	for (int i = 1; i <= numSlice + 1; i++) {
		float theta = (i - 1) * anglePerSlice;

		v.pos = glm::vec3(radius * glm::cos(theta), 0, radius * glm::sin(theta));

		vertex_buffer_data.push_back(v);
	}

	for (int i = 1; i <= numSlice + 1; i++) {
		index_buffer_data.push_back(i);
		index_buffer_data.push_back(0);
	}

	// Create the new mesh
	Mesh* mesh = new Mesh(meshName);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex),
		&vertex_buffer_data[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint),
		&index_buffer_data[0], GL_STATIC_DRAW);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLE_STRIP;

	return mesh;


}

Mesh* MeshBuilder::GenerateSphere(const std::string& meshName, glm::vec3 color, float radius, int numSlice, int numStack)
{
	Vertex v; 			                    // Vertex definition
	std::vector<Vertex> vertex_buffer_data; // Vertex Buffer Objects (VBOs)
	std::vector<GLuint> index_buffer_data;  // Element Buffer Objects (EBOs)

	v.color = color;

	float degreePerStack = glm::pi<float>() / numStack;
	float degreePerSlice = glm::two_pi<float>() / numSlice;

	for (unsigned stack = 0; stack < numStack + 1; ++stack) //stack
	{
		float phi = -glm::half_pi<float>() + stack * degreePerStack;
		for (unsigned slice = 0; slice < numSlice + 1; ++slice) //slice
		{
			float theta = slice * degreePerSlice;
			v.pos = glm::vec3(radius * glm::cos(phi) * glm::cos(theta),
				radius * glm::sin(phi),
				radius * glm::cos(phi) * glm::sin(theta));

			v.normal = glm::vec3(glm::cos(phi) * glm::cos(theta), glm::sin(phi), glm::cos(phi) * glm::sin(theta));

			vertex_buffer_data.push_back(v);
		}
	}

	for (unsigned stack = 0; stack < numStack; ++stack)
	{
		for (unsigned slice = 0; slice < numSlice + 1; ++slice)
		{
			index_buffer_data.push_back((numStack + 1) * stack + slice);
			index_buffer_data.push_back((numStack + 1) * (stack + 1) + slice);
		}
	}

	// Create the new mesh
	Mesh* mesh = new Mesh(meshName);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex),
		&vertex_buffer_data[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint),
		&index_buffer_data[0], GL_STATIC_DRAW);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLE_STRIP;

	return mesh;
}

Mesh* MeshBuilder::GenerateTorus(const std::string& meshName, glm::vec3 color, float innerR, float outerR, int numSlice, int numStack)
{
	Vertex v;									//Vertex definition
	std::vector<Vertex> vertex_buffer_data;		//Vertex Buffer Object (VBOs)
	std::vector<GLuint> index_buffer_data;

	v.color = color;

	float degreePerStack = glm::two_pi<float>() / numStack;
	float degreePerSlice = glm::two_pi<float>() / numSlice;

	for (unsigned stack = 0; stack < numStack + 1; ++stack)		//Stack
	{
		float phi = stack * degreePerStack;

		for (unsigned slice = 0; slice < numSlice + 1; ++slice)	//Slice
		{
			float theta = slice * degreePerSlice;
			v.pos = glm::vec3((outerR + innerR * glm::cos(theta)) * glm::sin(phi), innerR* glm::sin(theta), (outerR + innerR * glm::cos(theta))* glm::cos(phi));
			vertex_buffer_data.push_back(v);
		}
	}

	for (unsigned stack = 0; stack < numStack; stack ++)
	{
		for (unsigned slice = 0; slice < numSlice + 1; slice++)
		{
			index_buffer_data.push_back((numStack + 1) * stack + slice);
			index_buffer_data.push_back((numStack + 1) * (stack + 1) + slice);
		}
	}

	//Create the new mesh
	Mesh* mesh = new Mesh(meshName);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLE_STRIP;

	return mesh;
}
