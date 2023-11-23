#include "SceneLight.h"
#include "GL\glew.h"

// GLM Headers
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_inverse.hpp>

//Include GLFW
#include <GLFW/glfw3.h>

#include "KeyboardController.h"

#include "shader.hpp"
#include "Application.h"
#include "MeshBuilder.h"


#include <iostream>

SceneLight::SceneLight()
{

}

SceneLight::~SceneLight()
{

}

void SceneLight::Init()
{
	// Initialise camera properties
	camera.Init(45.f, 45.f, 10.f);

	// Set background color to dark blue
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	//Enable depth buffer and depth testing
	glEnable(GL_DEPTH_TEST);

	// Enable back face culling
	glEnable(GL_CULL_FACE);

	//Default to fill mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	m_programID = LoadShaders("Shader//Shading.vertexshader",
		"Shader//Shading.fragmentshader");
	glUseProgram(m_programID);

	// Get a handle for our "MVP" uniform
	{
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");
	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	}

	// Load identity matrix into the model stack
	modelStack.LoadIdentity();

	// Calculate the light position in camera space
	glm::vec3 lightPosition_cameraspace = viewStack.Top() * glm::vec4(light[0].position, 1);
	glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, glm::value_ptr(lightPosition_cameraspace));

	// Init VBO here
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = nullptr;
	}

	glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 1000.0f);
	projectionStack.LoadMatrix(projection);

	//Generate the basic mnesh
	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("Axes", 10000.f, 10000.f, 10000.f);

	meshList[GEO_QUAD] = MeshBuilder::GenerateQuad("Quad", glm::vec3(1,1,1), 1);

	//1.
	meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("Joints", glm::vec3(1.f, 1.f, 1.f), 1.f, 6, 6);
	//2.
	meshList[GEO_TORUS] = MeshBuilder::GenerateTorus("Torus", glm::vec3(1, 1, 1), 0.5f, 1, 8, 8);
	//3.
	meshList[GEO_TORUS_01] = MeshBuilder::GenerateTorus("Torus_01", glm::vec3(1, 1, 1), 0.5f, 0.35f, 8, 8);
	//4.
	meshList[GEO_CYLINDER] = MeshBuilder::GenerateCylinder("Limbs", glm::vec3(1, 1, 1), 1, 1, 2, 12);
	//5.
	meshList[GEO_CUBE] = MeshBuilder::GenerateCube("Legs",glm::vec3(1, 1, 1), 1, 1, 2, 4);

	// Init default data on start
	{
	headRotation = 1.0f;
	earthRotation = 1.0f;
	sunRotation = 1.0f;

	currAnim = ANIM_SUN;
	enableLight = false;

	light[0].position = glm::vec3(0, 5, 0);
	light[0].color = glm::vec3(1, 1, 1);
	light[0].power = 1;
	light[0].kC = 1.f;
	light[0].kL = 0.01f;
	light[0].kQ = 0.001f;

	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, glm::value_ptr(light[0].color));
	glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);

	eyeSize = 0.3f;
	speed = 75;
	}
}

void SceneLight::Update(double dt)
{
	// Check for key press
	HandleKeyPress();
	camera.Update(dt);

	if (headRotation >= 100 || headRotation <= -100)
	{
		speed = -speed;
	}
	std::cout << "Speed: " << headRotation << '\n';
	headRotation += static_cast<float>(dt) * speed;
	/*sunRotation += static_cast<float>(dt) * 10.f;
	earthRotation += static_cast<float>(dt) * 10.f;
	moonRotation += static_cast<float>(dt) * 50.f;*/

	//Do animation here
	switch (currAnim) 
	{
	case ANIM_MOON:
		
		break;

	case ANIM_EARTH:
		
		break;

	case ANIM_SUN:
		
		break;

	case ANIM_IDLE:
		//Idle anim

		break;

	case ANIM_WAVING:
		//Waving anim

		break;

	case ANIM_SWIMMING:
		//Swin anim

		break;

	case ANIM_SUMMERSAULT:
		//Summersault anim

		break;

	case ANIM_COMBO_ATTACK:
		//Combo attack anim

		break;

	default:
		break;
	}

	// Controller for movement of light source
	{
	if (KeyboardController::GetInstance()->IsKeyDown('I'))
		light[0].position.z -= static_cast<float>(dt) * 5.f;
	if (KeyboardController::GetInstance()->IsKeyDown('K'))
		light[0].position.z += static_cast<float>(dt) * 5.f;
	if (KeyboardController::GetInstance()->IsKeyDown('J'))
		light[0].position.x -= static_cast<float>(dt) * 5.f;
	if (KeyboardController::GetInstance()->IsKeyDown('L'))
		light[0].position.x += static_cast<float>(dt) * 5.f;
	if (KeyboardController::GetInstance()->IsKeyDown('O'))
		light[0].position.y -= static_cast<float>(dt) * 5.f;
	if (KeyboardController::GetInstance()->IsKeyDown('P'))
		light[0].position.y += static_cast<float>(dt) * 5.f;
	}
}

void SceneLight::Render()
{
	// Clear color buffer every frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//// Setup Model View Projection matrix
	/*glm::mat4 model = glm::mat4(1.f);
	glm::mat4 view = glm::lookAt(camera.position, camera.target, camera.up);
	glm::mat4 projection = glm::perspective(glm::radians(60.f), 4.f / 3.f, 0.1f, 10000.f);*/

	//// Define the projection matrix
	//switch (projType)
	//{
	//case 0: // ORTHOGRAPHICS
	//	// Add codes here to setup orthographic matrix
	//	projection = glm::ortho(-40.f, 40.f, -60.f, 60.f, 0.1f, 1000.f);
	//	break;
	//default: // PERSPECTIVE
	//	// Add codes here to setup perspective matrix
	//	projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.1f, 10000.f);
	//}

	// Calculate the Model-View-Project matrix
	/*glm::mat4 MVP = projection * view * model;
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, glm::value_ptr(MVP));*/

	// Calculate the Model-View-Projection matrix
	//glm::mat4 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	//glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, glm::value_ptr(MVP));

	//// Render objects
	//meshList[GEO_AXES]->Render();
	//meshList[GEO_QUAD]->Render();
	//meshList[GEO_CIRCLE]->Render();
	//meshList[GEO_SPHERE]->Render();
	//meshList[GEO_TORUS]->Render();

	// Load view matrix stack and set it with camera position, target position and up direction
	viewStack.LoadIdentity();
	viewStack.LookAt(camera.position.x, camera.position.y, camera.position.z,camera.target.x, camera.target.y, camera.target.z,camera.up.x, camera.up.y, camera.up.z );

	// Load identity matrix into the model stack
	modelStack.LoadIdentity();

	// Calculate the light position in camera space
	glm::vec3 lightPosition_cameraspace = viewStack.Top() * glm::vec4(light[0].position, 1);
	glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, glm::value_ptr(lightPosition_cameraspace));

	modelStack.PushMatrix();
	// Render objects
	RenderMesh(meshList[GEO_AXES], false);
	modelStack.PopMatrix();
	
	// Render light
	{
		modelStack.PushMatrix();
		modelStack.Translate(light[0].position.x, light[0].position.y + 4, light[0].position.z);
		modelStack.Scale(0.1f, 0.1f, 0.1f);
		meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
		meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
		meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
		meshList[GEO_SPHERE]->material.kShininess = 3.0f;
		RenderMesh(meshList[GEO_SPHERE], enableLight);
		modelStack.PopMatrix();
	}

	//Whole upper body
	{
		//Render of pelvis
		modelStack.PushMatrix();

		//Rotation of the whole boday
		modelStack.Rotate(0, 1, 0, 0);
		{
			//Render area of head
			{
				//Render of neck
				modelStack.PushMatrix();
				modelStack.Rotate(headRotation, 0, 1, 0);
				//Render of the head
				{
					modelStack.PushMatrix();
					//Render the of two eye
					{
						//Render the of left eye
						modelStack.PushMatrix();
						{
							//Eye ball left
							modelStack.PushMatrix();
							modelStack.Translate(-1, 2.2f, 1.1f);
							modelStack.Scale(eyeSize, eyeSize, eyeSize);
							modelStack.Rotate(90, 1, 0, 0);
							meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(1, 0.9f, 0.4f);
							meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
							meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
							meshList[GEO_SPHERE]->material.kShininess = 1.0f;
							RenderMesh(meshList[GEO_SPHERE], true);

							modelStack.PopMatrix();
						}
						modelStack.Translate(-1, 2.2f, 1.15f);
						modelStack.Scale(eyeSize, eyeSize, eyeSize);
						modelStack.Rotate(90, 1, 0, 0);
						modelStack.Rotate(45, 0, 0, 1);

						meshList[GEO_TORUS]->material.kAmbient = glm::vec3(0.7f, 0.4f, 0.f);
						meshList[GEO_TORUS]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
						meshList[GEO_TORUS]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
						meshList[GEO_TORUS]->material.kShininess = 3.0f;
						RenderMesh(meshList[GEO_TORUS], true);
						modelStack.PopMatrix();

						//Render the of right eye
						modelStack.PushMatrix();
						{
							//Eye ball right
							modelStack.PushMatrix();
							modelStack.Translate(1, 2.2f, 1.1f);
							modelStack.Scale(eyeSize, eyeSize, eyeSize);
							modelStack.Rotate(90, 1, 0, 0);
							meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(1, 0.9f, 0.4f);
							meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
							meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
							meshList[GEO_SPHERE]->material.kShininess = 1.0f;
							RenderMesh(meshList[GEO_SPHERE], true);
							modelStack.PopMatrix();
						}
						modelStack.Translate(1, 2.2f, 1.15f);
						modelStack.Scale(eyeSize, eyeSize, eyeSize);
						modelStack.Rotate(90, 1, 0, 0);
						modelStack.Rotate(-45, 0, 0, 1);

						meshList[GEO_TORUS]->material.kAmbient = glm::vec3(0.7f, 0.4f, 0.f);
						meshList[GEO_TORUS]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
						meshList[GEO_TORUS]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
						meshList[GEO_TORUS]->material.kShininess = 3.0f;
						RenderMesh(meshList[GEO_TORUS], true);
						modelStack.PopMatrix();
					}
					modelStack.Translate(0, 2, 0);
					modelStack.Scale(1.9f, 1.5f, 1.5f);

					meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(0.25f, 0.25f, 0.03f);
					meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.4f, 0.4f, 0.4f);
					meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.6f, 0.6f, 0.6f);
					meshList[GEO_SPHERE]->material.kShininess = 1.0f;
					RenderMesh(meshList[GEO_SPHERE], true);
					modelStack.PopMatrix();
				}
				modelStack.Translate(0, 0.45f, 0);
				modelStack.Scale(0.35f, 0.35f, 0.35f);

				meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(1, 0.2f, 0);
				meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
				meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
				meshList[GEO_SPHERE]->material.kShininess = 1.0f;
				RenderMesh(meshList[GEO_SPHERE], true);
				modelStack.PopMatrix();
			}

			//Render area of upper body
			{
				//Render of chest

				modelStack.PushMatrix();
				modelStack.Translate(0, -0.9f, 0);
				modelStack.Scale(1.25f, 2.65f, 1.25f);
				meshList[GEO_TORUS_01]->material.kAmbient = glm::vec3(0.75f, 0.25f, 0.03f);
				meshList[GEO_TORUS_01]->material.kDiffuse = glm::vec3(0.4f, 0.4f, 0.4f);
				meshList[GEO_TORUS_01]->material.kSpecular = glm::vec3(0.6f, 0.6f, 0.6f);
				meshList[GEO_TORUS_01]->material.kShininess = 1.0f;
				RenderMesh(meshList[GEO_TORUS_01], true);
				modelStack.PopMatrix();

			}

			//Render of left hand
			{
				//Left shoulder Joints
				modelStack.PushMatrix();
				{
					//Left upper limbs
					modelStack.PushMatrix();
					{
						//Left hand - Mid joint
						modelStack.PushMatrix();
						{
							//Left lower limbs
							modelStack.PushMatrix();
							{
								//Left paw joint
								modelStack.PushMatrix();
								modelStack.Translate(-3.65f, 0, 0);
								modelStack.Scale(0.35f, 0.35f, 0.35f);
								meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(1, 0.2f, 0);
								meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
								meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
								meshList[GEO_SPHERE]->material.kShininess = 1.0f;
								RenderMesh(meshList[GEO_SPHERE], true);
								modelStack.PopMatrix();
							}
							modelStack.Translate(-2.55f, 0, 0);
							modelStack.Scale(0.5f, 0.25f, 0.25f);
							modelStack.Rotate(90, 0, 0, 1);
							meshList[GEO_CYLINDER]->material.kAmbient = glm::vec3(1, 0.2f, 0);
							meshList[GEO_CYLINDER]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
							meshList[GEO_CYLINDER]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
							meshList[GEO_CYLINDER]->material.kShininess = 1.0f;
							RenderMesh(meshList[GEO_CYLINDER], true);
							modelStack.PopMatrix();
						}
						modelStack.Translate(-2.45f, 0, 0);
						modelStack.Scale(0.35f, 0.35f, 0.35f);
						meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(1, 0.2f, 0);
						meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
						meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
						meshList[GEO_SPHERE]->material.kShininess = 1.0f;
						RenderMesh(meshList[GEO_SPHERE], true);
						modelStack.PopMatrix();
					}
					modelStack.Translate(-1.35f, 0, 0);
					modelStack.Scale(0.5f, 0.25f, 0.25f);
					modelStack.Rotate(90, 0, 0, 1);
					meshList[GEO_CYLINDER]->material.kAmbient = glm::vec3(1, 0.2f, 0);
					meshList[GEO_CYLINDER]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
					meshList[GEO_CYLINDER]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
					meshList[GEO_CYLINDER]->material.kShininess = 1.0f;
					RenderMesh(meshList[GEO_CYLINDER], true);
					modelStack.PopMatrix();
				}
				modelStack.Translate(-1.2f, 0, 0);
				modelStack.Scale(0.35f, 0.35f, 0.35f);
				meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(1, 0.2f, 0);
				meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
				meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
				meshList[GEO_SPHERE]->material.kShininess = 1.0f;
				RenderMesh(meshList[GEO_SPHERE], true);
				modelStack.PopMatrix();
			}

			//Render of right hand
			{
				//Right shoulder Joints
				modelStack.PushMatrix();
				{
					//Right upper limbs
					modelStack.PushMatrix();
					{
						//Right hand - Mid joint
						modelStack.PushMatrix();
						{
							//Right lower limbs
							modelStack.PushMatrix();
							{
								//Right paw joint
								modelStack.PushMatrix();
								modelStack.Translate(3.65f, 0, 0);
								modelStack.Scale(0.35f, 0.35f, 0.35f);
								meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(1, 0.2f, 0);
								meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
								meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
								meshList[GEO_SPHERE]->material.kShininess = 1.0f;
								RenderMesh(meshList[GEO_SPHERE], true);
								modelStack.PopMatrix();
							}
							modelStack.Translate(2.55f, 0, 0);
							modelStack.Scale(0.5f, 0.25f, 0.25f);
							modelStack.Rotate(-90, 0, 0, 1);
							meshList[GEO_CYLINDER]->material.kAmbient = glm::vec3(1, 0.2f, 0);
							meshList[GEO_CYLINDER]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
							meshList[GEO_CYLINDER]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
							meshList[GEO_CYLINDER]->material.kShininess = 1.0f;
							RenderMesh(meshList[GEO_CYLINDER], true);
							modelStack.PopMatrix();
						}
						modelStack.Translate(2.45f, 0, 0);
						modelStack.Scale(0.35f, 0.35f, 0.35f);
						meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(1, 0.2f, 0);
						meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
						meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
						meshList[GEO_SPHERE]->material.kShininess = 1.0f;
						RenderMesh(meshList[GEO_SPHERE], true);
						modelStack.PopMatrix();
					}
					modelStack.Translate(1.35f, 0, 0);
					modelStack.Scale(0.5f, 0.25f, 0.25f);
					modelStack.Rotate(-90, 0, 0, 1);
					meshList[GEO_CYLINDER]->material.kAmbient = glm::vec3(1, 0.2f, 0);
					meshList[GEO_CYLINDER]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
					meshList[GEO_CYLINDER]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
					meshList[GEO_CYLINDER]->material.kShininess = 1.0f;
					RenderMesh(meshList[GEO_CYLINDER], true);
					modelStack.PopMatrix();
				}
				modelStack.Translate(1.2f, 0, 0);
				modelStack.Scale(0.35f, 0.35f, 0.35f);
				meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(1, 0.2f, 0);
				meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
				meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
				meshList[GEO_SPHERE]->material.kShininess = 1.0f;
				RenderMesh(meshList[GEO_SPHERE], true);
				modelStack.PopMatrix();
			}

			//Render of left leg
			{
				//Left leg Joints
				modelStack.PushMatrix();
				{
					//Left thigh
					modelStack.PushMatrix();
					{
						//Left Knee
						modelStack.PushMatrix();
						{
							//Left calf
							modelStack.PushMatrix();
							{
								//Left ankle
								modelStack.PushMatrix();
								{
									//Left Legs
									modelStack.PushMatrix();
									modelStack.Translate(-1.05f, -4.5, 0.1f);
									modelStack.Scale(0.55f, 0.15f, 0.55f);
									modelStack.Rotate(35, 0, 1, 0);
									meshList[GEO_CUBE]->material.kAmbient = glm::vec3(1, 0.2f, 0);
									meshList[GEO_CUBE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
									meshList[GEO_CUBE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
									meshList[GEO_CUBE]->material.kShininess = 1.0f;
									RenderMesh(meshList[GEO_CUBE], true);
									modelStack.PopMatrix();
								}
								modelStack.Translate(-1.f, -4, 0);
								modelStack.Scale(0.35f, 0.35f, 0.35f);
								meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(1, 0.2f, 0);
								meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
								meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
								meshList[GEO_SPHERE]->material.kShininess = 1.0f;
								RenderMesh(meshList[GEO_SPHERE], true);
								modelStack.PopMatrix();
							}
							modelStack.Translate(-1.f, -4.15f, 0);
							modelStack.Scale(0.25f, 0.65f, 0.25f);
							meshList[GEO_CYLINDER]->material.kAmbient = glm::vec3(1, 0.2f, 0);
							meshList[GEO_CYLINDER]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
							meshList[GEO_CYLINDER]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
							meshList[GEO_CYLINDER]->material.kShininess = 1.0f;
							RenderMesh(meshList[GEO_CYLINDER], true);
							modelStack.PopMatrix();
						}
						modelStack.Translate(-1.f, -3, 0);
						modelStack.Scale(0.35f, 0.35f, 0.35f);
						meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(1, 0.2f, 0);
						meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
						meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
						meshList[GEO_SPHERE]->material.kShininess = 1.0f;
						RenderMesh(meshList[GEO_SPHERE], true);
						modelStack.PopMatrix();

					}
					modelStack.Translate(-1.f, -3.15f, 0);
					modelStack.Scale(0.25f, 0.65f, 0.25f);
					meshList[GEO_CYLINDER]->material.kAmbient = glm::vec3(1, 0.2f, 0);
					meshList[GEO_CYLINDER]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
					meshList[GEO_CYLINDER]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
					meshList[GEO_CYLINDER]->material.kShininess = 1.0f;
					RenderMesh(meshList[GEO_CYLINDER], true);
					modelStack.PopMatrix();
				}
				modelStack.Translate(-1.f, -2, 0);
				modelStack.Scale(0.35f, 0.35f, 0.35f);
				meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(1, 0.2f, 0);
				meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
				meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
				meshList[GEO_SPHERE]->material.kShininess = 1.0f;
				RenderMesh(meshList[GEO_SPHERE], true);
				modelStack.PopMatrix();
			}

			//Render of right leg
			{
				//Right leg Joints
				modelStack.PushMatrix();
				{
					//Right thigh
					modelStack.PushMatrix();
					{
						//Right Knee
						modelStack.PushMatrix();
						{
							//Right calf
							modelStack.PushMatrix();
							{
								//Right ankle
								modelStack.PushMatrix();
								{
									//Right Legs
									modelStack.PushMatrix();
									modelStack.Translate(1.05f, -4.5, 0.1f);
									modelStack.Scale(0.55f, 0.15f, 0.55f);
									modelStack.Rotate(-35, 0, 1, 0);
									meshList[GEO_CUBE]->material.kAmbient = glm::vec3(1, 0.2f, 0);
									meshList[GEO_CUBE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
									meshList[GEO_CUBE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
									meshList[GEO_CUBE]->material.kShininess = 1.0f;
									RenderMesh(meshList[GEO_CUBE], true);
									modelStack.PopMatrix();
								}
								modelStack.Translate(1.f, -4, 0);
								modelStack.Scale(0.35f, 0.35f, 0.35f);
								meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(1, 0.2f, 0);
								meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
								meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
								meshList[GEO_SPHERE]->material.kShininess = 1.0f;
								RenderMesh(meshList[GEO_SPHERE], true);
								modelStack.PopMatrix();
							}
							modelStack.Translate(1.f, -4.15f, 0);
							modelStack.Scale(0.25f, 0.65f, 0.25f);
							meshList[GEO_CYLINDER]->material.kAmbient = glm::vec3(1, 0.2f, 0);
							meshList[GEO_CYLINDER]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
							meshList[GEO_CYLINDER]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
							meshList[GEO_CYLINDER]->material.kShininess = 1.0f;
							RenderMesh(meshList[GEO_CYLINDER], true);
							modelStack.PopMatrix();
						}
						modelStack.Translate(1.f, -3, 0);
						modelStack.Scale(0.35f, 0.35f, 0.35f);
						meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(1, 0.2f, 0);
						meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
						meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
						meshList[GEO_SPHERE]->material.kShininess = 1.0f;
						RenderMesh(meshList[GEO_SPHERE], true);
						modelStack.PopMatrix();
					}
					modelStack.Translate(1.f, -3.15f, 0);
					modelStack.Scale(0.25f, 0.65f, 0.25f);
					meshList[GEO_CYLINDER]->material.kAmbient = glm::vec3(1, 0.2f, 0);
					meshList[GEO_CYLINDER]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
					meshList[GEO_CYLINDER]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
					meshList[GEO_CYLINDER]->material.kShininess = 1.0f;
					RenderMesh(meshList[GEO_CYLINDER], true);
					modelStack.PopMatrix();
				}
				modelStack.Translate(1.f, -2, 0);
				modelStack.Scale(0.35f, 0.35f, 0.35f);
				meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(1, 0.2f, 0);
				meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
				meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
				meshList[GEO_SPHERE]->material.kShininess = 1.0f;
				RenderMesh(meshList[GEO_SPHERE], true);
				modelStack.PopMatrix();

			}

		}

		modelStack.Translate(0, -1.8f, 0);
		modelStack.Scale(.4f, .4f, .4f);
		meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(1, 0.9f, 0.4f);
		meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
		meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
		meshList[GEO_SPHERE]->material.kShininess = 1.0f;
		RenderMesh(meshList[GEO_SPHERE], true);

		modelStack.PopMatrix();
	}
}

void SceneLight::Exit()
{
	// Cleanup VBO here
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		if (meshList[i])
		{
			delete meshList[i];
		}
	}
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}

void SceneLight::HandleKeyPress() 
{
	if (Application::IsKeyPressed(0x31))
	{
		// Key press to enable culling
		glEnable(GL_CULL_FACE);
	}
	if (Application::IsKeyPressed(0x32))
	{
		// Key press to disable culling
		glDisable(GL_CULL_FACE);
	}
	if (Application::IsKeyPressed(0x33))
	{
		// Key press to enable fill mode for the polygon
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //default fill mode
	}
	if (Application::IsKeyPressed(0x34))
	{
		// Key press to enable wireframe mode for the polygon
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe mode
	}
	if (KeyboardController::GetInstance()->IsKeyPressed('P'))
	{
		if (projType == 0)
		{
			std::cout << "Press";
			projType = 1; // Perspective
		}
			
		else
		{
			projType = 0; // Orthographic
		}	
	}

	if (KeyboardController::GetInstance()->IsKeyPressed('E'))
	{
		// Only animate the earth
		currAnim = ANIM_EARTH;
	}

	if (KeyboardController::GetInstance()->IsKeyPressed('M'))
	{
		// Only animte the moon
		currAnim = ANIM_MOON;
	}

	if (KeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_ENTER))
	{
		// Only animate the sun
		currAnim = ANIM_SUN;
		
	}

	if (KeyboardController::GetInstance()->IsKeyPressed('R'))
	{
		// Reset everything to original state
		currAnim = ANIM_SUN;
		headRotation = 0.0f;
		earthRotation = 0.0f;
		sunRotation = 0.0f;
	}

	if (KeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_0))
	{
		// Toggle light on or off
		std::cout << "Light !" << enableLight <<'\n';
		enableLight = !enableLight;
	}

	//Start idle animation
	if (KeyboardController::GetInstance()->IsKeyPressed('Z'))
	{
		currAnim = ANIM_IDLE;
		std::cout <<"IDLING" << '\n';
	}
	if (KeyboardController::GetInstance()->IsKeyPressed('X'))
	{
		currAnim = ANIM_WAVING;
		std::cout << "Waving !" << '\n';
	}
	if (KeyboardController::GetInstance()->IsKeyPressed('C'))
	{
		currAnim = ANIM_SWIMMING;
		std::cout << "Swimming !" << '\n';
	}
	if (KeyboardController::GetInstance()->IsKeyPressed('V'))
	{
		currAnim = ANIM_SUMMERSAULT;
		std::cout << "Summersaulting !" << '\n';
	}
	if (KeyboardController::GetInstance()->IsKeyPressed('B'))
	{
		currAnim = ANIM_COMBO_ATTACK;
		std::cout << "Combo attacking !" << '\n';
	}
}

void SceneLight::RenderMesh(Mesh* mesh, bool enableLight)
{
	glm::mat4 MVP, modelView, modelView_inverse_transpose;

	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, glm::value_ptr(MVP));
	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, glm::value_ptr(modelView));

	if (enableLight)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		modelView_inverse_transpose = glm::inverseTranspose(modelView);
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE,
			glm::value_ptr(modelView_inverse_transpose));

		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, glm::value_ptr(mesh->material.kAmbient));
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, glm::value_ptr(mesh->material.kDiffuse));
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, glm::value_ptr(mesh->material.kSpecular));
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}
	mesh->Render();
}

