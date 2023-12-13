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
	v.pos = glm::vec3(-0.5f * length, 0.5f * length, 0.f);	v.normal = glm::vec3(0, 0, 1); v.texCoord = glm::vec2(0, 1);  vertex_buffer_data.push_back(v); 
	v.pos = glm::vec3(-0.5f * length, -0.5f * length, 0.f);	v.normal = glm::vec3(0, 0, 1); v.texCoord = glm::vec2(0, 0); vertex_buffer_data.push_back(v); //v2
	v.pos = glm::vec3(0.5f * length, 0.5f * length, 0.f);	v.normal = glm::vec3(0, 0, 1);  v.texCoord = glm::vec2(1, 1); vertex_buffer_data.push_back(v); //v3
	v.pos = glm::vec3(0.5f * length, -0.5f * length, 0.f);	v.normal = glm::vec3(0, 0, 1);  v.texCoord = glm::vec2(1, 0); vertex_buffer_data.push_back(v); //v4

	//tri1
	index_buffer_data.push_back(0);
	index_buffer_data.push_back(1);
	index_buffer_data.push_back(2);
	index_buffer_data.push_back(3);

	// Create the new mesh
	Mesh* mesh = new Mesh(meshName);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLE_STRIP;

	return mesh;
}


/******************************************************************************/
Mesh* MeshBuilder::GenerateCylinder(const std::string& meshName, glm::vec3 color, float topRadius, float btmRadius, int height, int numSlice)
{
	Vertex v;                              // Vertex definition
	std::vector<Vertex> vertex_buffer_data;// Vertex Buffer Object ( VBOs )
	std::vector<GLuint> index_buffer_data; // Element Buffer Object ( EBOs )

	v.color = color;

	float anglePerSlice = glm::two_pi<float>() / numSlice;

	float phi = numSlice * anglePerSlice;

	v.pos = glm::vec3(0, 0, 0);
	vertex_buffer_data.push_back(v);

	//Bottom circle
	for (int i = 0; i < numSlice; i++) 
	{
		float theta = i * anglePerSlice;
		v.pos = glm::vec3(btmRadius * glm::cos(theta), 0, btmRadius * glm::sin(theta));

		v.normal = glm::vec3(glm::cos(phi) * glm::cos(theta), glm::sin(phi), glm::cos(phi) * glm::sin(theta));
		
		vertex_buffer_data.push_back(v);
	}

	v.pos = glm::vec3(0, 0, 0);
	vertex_buffer_data.push_back(v);
	//Top circle
	for (int y = 0; y < numSlice; y++)
	{
		float theta = y * anglePerSlice;

		v.pos = glm::vec3(topRadius * glm::cos(theta), height, topRadius * glm::sin(theta));

		v.normal = glm::vec3(glm::cos(phi) * glm::cos(theta), glm::sin(phi), glm::cos(phi) * glm::sin(theta));

		vertex_buffer_data.push_back(v);
	}

	for (int i = 1; i <= numSlice; i++)
	{	
		// Bottom circle
		index_buffer_data.push_back(0);
		index_buffer_data.push_back(i);
		index_buffer_data.push_back((i % numSlice) + 1);

		// Top circle
		index_buffer_data.push_back(numSlice);
		index_buffer_data.push_back(numSlice +i);
		index_buffer_data.push_back(numSlice + (i % numSlice) + 1);

		index_buffer_data.push_back(numSlice + 2);									//5,5, 5
		index_buffer_data.push_back(numSlice + 1 + ((i % numSlice) + 1));			//7,
		index_buffer_data.push_back(numSlice + 1 + i);

		// Side faces
		index_buffer_data.push_back(i);						// 1,2,3
		index_buffer_data.push_back((i % numSlice) + 1);	//2,3,4
		index_buffer_data.push_back(numSlice + 1 + i);		//5,6,7

		index_buffer_data.push_back((i % numSlice) + 1);
		index_buffer_data.push_back(numSlice + 1 + ((i % numSlice) + 1));
		index_buffer_data.push_back(numSlice + 1 + i);
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
			v.normal = glm::vec3(glm::cos(phi) * glm::cos(theta), glm::sin(phi), glm::cos(phi) * glm::sin(theta));
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

Mesh* MeshBuilder::GenerateCube(const std::string& meshName, glm::vec3 color, float topRadius, float btmRadius, int height, int numSlice)
{
	Vertex v;                              // Vertex definition
	std::vector<Vertex> vertex_buffer_data;// Vertex Buffer Object ( VBOs )
	std::vector<GLuint> index_buffer_data; // Element Buffer Object ( EBOs )

	v.color = color;

	float anglePerSlice = glm::two_pi<float>() / numSlice;

	v.pos = glm::vec3(0, 0, 0);
	vertex_buffer_data.push_back(v);

	//Bottom circle
	for (int i = 0; i < numSlice; i++)
	{
		float theta = i * anglePerSlice;
		v.pos = glm::vec3(btmRadius * glm::cos(theta), 0, btmRadius * glm::sin(theta));


		vertex_buffer_data.push_back(v);
	}

	v.pos = glm::vec3(0, 0, 0);
	vertex_buffer_data.push_back(v);
	//Top circle
	for (int y = 0; y < numSlice; y++)
	{
		float theta = y * anglePerSlice;

		v.pos = glm::vec3(topRadius * glm::cos(theta), height, topRadius * glm::sin(theta));

		vertex_buffer_data.push_back(v);
	}

	for (int i = 1; i <= numSlice; i++)
	{
		// Bottom circle
		index_buffer_data.push_back(0);
		index_buffer_data.push_back(i);
		index_buffer_data.push_back((i % numSlice) + 1);

		// Top circle
		index_buffer_data.push_back(numSlice);
		index_buffer_data.push_back(numSlice + i);
		index_buffer_data.push_back(numSlice + (i % numSlice) + 1);

		index_buffer_data.push_back(numSlice + 2);									//5,5, 5
		index_buffer_data.push_back(numSlice + 1 + ((i % numSlice) + 1));			//7,
		index_buffer_data.push_back(numSlice + 1 + i);

		// Side faces
		index_buffer_data.push_back(i);						// 1,2,3
		index_buffer_data.push_back((i % numSlice) + 1);	//2,3,4
		index_buffer_data.push_back(numSlice + 1 + i);		//5,6,7

		index_buffer_data.push_back((i % numSlice) + 1);
		index_buffer_data.push_back(numSlice + 1 + ((i % numSlice) + 1));
		index_buffer_data.push_back(numSlice + 1 + i);
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
