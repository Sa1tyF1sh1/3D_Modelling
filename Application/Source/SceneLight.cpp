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

void SceneLight::setDefaultValue() 
{
	//Common
	jointSize = 0.35f;
	eyeSize = 0.3f;
	upperBodySize = 1.25f;

	//Values
	headRotateAmt = 0;
	rightHandTranslateAmt = 0;
	rightForearmRotAmt = 0;
	lefthandTranslateAmt = 0;
	leftlegRotateAmt = 0;
	rightlegRoteAmt = 0;
	legRoteAmt = 0;
	bodyMovementAmt_idle = 0;
	bodyMovementAmt_ss = 0;
	legMovementAmt_ss = 0;
	handRoteAmt = 0;
	bodyRotAmt = 0;
	bodyTransAmt = 0;
	punchCounter = 0;

	//Speeds
	rightHandRotateSpeed = 35;
	rightForearmRotSpeed = 40;
	headRotateSpeed = 20;
	legRotSpeed = 60;
	lefthandRotSpeed = 30;
	bodyMovementSpeed_idle = 0.5f;
	bodyMovementSpeed_ss = 0.5f;
	legMovementSpeed_ss = 26;
	handRoteSpeed = 26;
	bodyRotSpeed = 150;
	bodyTransSped = 5;
	bodyRotSpeed_ca = 20;
	rightHandTranslateSpeed = 40;
	leftHandTranslateSpeed = 40;

	//Conditions
	finishAnim_SS = true;
	rightHandAnim = true;
	leftHandAnim = false;
	kickAnim = false;
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
	currAnim = ANIM_DEFAULT;

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

	//Common
	jointSize = 0.35f;
	eyeSize = 0.3f;
	upperBodySize = 1.25f;
	
	//Values
	headRotateAmt = 0;
	rightHandTranslateAmt = 0;
	rightForearmRotAmt = 0;
	lefthandTranslateAmt = 0;
	leftlegRotateAmt = 0;
	rightlegRoteAmt = 0;
	legRoteAmt = 0;
	bodyMovementAmt_idle = 0;
	bodyMovementAmt_ss = 0;
	legMovementAmt_ss = 0;
	handRoteAmt = 0;
	bodyRotAmt = 0;
	bodyTransAmt = 0;
	punchCounter = 0;

	//Speeds
	rightHandRotateSpeed = 35;
	rightForearmRotSpeed = 40;
	headRotateSpeed = 20;
	legRotSpeed = 60;
	lefthandRotSpeed = 30;
	bodyMovementSpeed_idle = 0.5f;
	bodyMovementSpeed_ss = 0.5f;
	legMovementSpeed_ss = 26;
	handRoteSpeed = 26;
	bodyRotSpeed = 150;
	bodyTransSped = 5;
	bodyRotSpeed_ca = 20;
	rightHandTranslateSpeed = 40;
	leftHandTranslateSpeed = 40;

	//Conditions
	finishAnim_SS = true;
	rightHandAnim = true;
	leftHandAnim = false;
	kickAnim = false;
	}
}

void SceneLight::Update(double dt)
{
	// Check for key press
	HandleKeyPress();
	camera.Update(dt);

	//Do animation here
	switch (currAnim) 
	{
		//Press 'Z'
	case ANIM_IDLE:
		//Idle anim
		lefthandTranslateAmt += static_cast<float>(dt) * lefthandRotSpeed;
		if (lefthandTranslateAmt >= 30 || lefthandTranslateAmt <= -30)
		{	
			lefthandRotSpeed *= -1;
		}
		bodyMovementAmt_idle += static_cast<float>(dt) * bodyMovementSpeed_idle;
		if (bodyMovementAmt_idle >= 0.25f || bodyMovementAmt_idle <= -0)
		{
			bodyMovementSpeed_idle *= -1;
		}

		break;
		//Press 'X'
	case ANIM_WAVING:
		//Waving anim
		//Head tilt to left
	
		headRotateAmt += static_cast<float>(dt) * headRotateSpeed;

		if (headRotateAmt >= 20)
		{
			headRotateSpeed = 0;
		}
		else
		{
			headRotateSpeed = 50;
		}
		
		//Waving
		rightHandTranslateAmt += static_cast<float>(dt) * rightHandRotateSpeed;
		
		if (rightHandTranslateAmt >= 40 || rightHandTranslateAmt <= 0)
		{
			rightHandRotateSpeed = -rightHandRotateSpeed;
		}
		rightForearmRotAmt += static_cast<float>(dt) * rightForearmRotSpeed;
		if (rightHandTranslateAmt >= 40 || rightHandTranslateAmt <= 0)
		{
			rightForearmRotSpeed = -rightForearmRotSpeed;
		}
		//Put down the left hand
		lefthandTranslateAmt -= static_cast<float>(dt) * lefthandRotSpeed;
		if (lefthandTranslateAmt >= 45)
		{
			lefthandRotSpeed = 0;
		}
		else
		{
			lefthandRotSpeed = -20;
		}
		break;
		//Press 'C'
	case ANIM_SWIMMING:
		
		//Head Swing
		if (headRotateAmt >= 30 || headRotateAmt <= -30)
		{
			headRotateSpeed = -headRotateSpeed;
		}
		headRotateAmt += static_cast<float>(dt) * headRotateSpeed;

		//Hand movement
		rightHandTranslateAmt += static_cast<float>(dt) * rightHandRotateSpeed;
		if (rightHandTranslateAmt >= 55 || rightHandTranslateAmt <= -55)
		{
			rightHandRotateSpeed = -rightHandRotateSpeed;
			if (rightHandTranslateAmt >= 55)
			{
				rightHandRotateSpeed *= 3;
			}
			else if (rightHandTranslateAmt <= -55)
			{
				rightHandRotateSpeed /= 3;
			}
		}
		legRoteAmt += static_cast<float>(dt) * legRotSpeed;
		if (legRoteAmt >= 50 || legRoteAmt <= -50)
		{
			legRotSpeed = -legRotSpeed;
		}

		//Leg movement

		break;
		//Press 'V'
	case ANIM_SUMMERSAULT:
		//Start animation while player not perform the summersault
		finishAnim_SS = false;
		if (!finishAnim_SS)
		{
			headRotateAmt += static_cast<float>(dt) * headRotateSpeed;
			bodyMovementAmt_ss += static_cast<float>(dt) * -bodyMovementSpeed_ss;
			legMovementAmt_ss += static_cast<float>(dt) * legMovementSpeed_ss;
			handRoteAmt += static_cast<float>(dt) * handRoteSpeed;

			if (headRotateAmt >= 30)
			{
				headRotateSpeed *= 0;
			}

			if (bodyMovementAmt_ss <= -1)
			{
				bodyMovementSpeed_ss *= 0;

				bodyTransAmt += static_cast<float>(dt) * bodyTransSped;
				bodyRotAmt += static_cast<float>(dt) * bodyRotSpeed;
				if (bodyRotAmt >= 360)
				{
					bodyRotSpeed *= 0;
					bodyTransSped *= 0;
					finishAnim_SS = true;
				}
				
			}

			if (legMovementAmt_ss >= 55)
			{
				legMovementSpeed_ss *= 0;
			}

			if (handRoteAmt >= 50)
			{
				handRoteSpeed *= 0;
			}
			
		}
		//Start animation to reverse to the orignal pose;

		if(finishAnim_SS)
		{
			std::cout << "Reverse Anim" << '\n';
			//headRotateAmt -= static_cast<float>(dt) * 20;
			//legMovementAmt_ss -= static_cast<float>(dt) * 26;
			//handRoteAmt -= static_cast<float>(dt) * 26;

			if (headRotateAmt >= 0)
			{
				headRotateAmt -= static_cast<float>(dt) * 20;
			}
			else
			{
				headRotateAmt -= static_cast<float>(dt) * 0;
			}

			if (legMovementAmt_ss >= 0)
			{
				legMovementAmt_ss -= static_cast<float>(dt) * 26;
			}
			else
			{
				legMovementAmt_ss -= static_cast<float>(dt) * 0;
			}

			if (handRoteAmt >= 0)
			{
				handRoteAmt -= static_cast<float>(dt) * 26;
			}
			else
			{
				handRoteAmt -= static_cast<float>(dt) * 0;
			}

		}

		break;
		//Press 'B'
	case ANIM_COMBO_ATTACK:
		bodyRotAmt += static_cast<float>(dt) * bodyRotSpeed_ca;
		if (bodyRotAmt >= 20)
		{
			bodyRotSpeed_ca *= 0;
		}
		//Punch
		handRoteAmt += static_cast<float>(dt) * handRoteSpeed;
		if (handRoteAmt >= 40)
		{
			handRoteAmt = 40;
			//leftHandAnim = true;
			if (leftHandAnim)
			{
				//rightHandTranslateAmt = 0;
				lefthandTranslateAmt += static_cast<float>(dt) * leftHandTranslateSpeed;
				if (lefthandTranslateAmt >= 20 || lefthandTranslateAmt <= 0)
				{
					leftHandTranslateSpeed = -leftHandTranslateSpeed;

					if (lefthandTranslateAmt <= 0)
					{
						punchCounter++;
						rightHandAnim = true;
						leftHandAnim = false;
					}
				}
			}
			if (rightHandAnim)
			{
				//lefthandTranslateAmt = 0;
				rightHandTranslateAmt += static_cast<float>(dt) * rightHandTranslateSpeed;
				if (rightHandTranslateAmt >= 20 || rightHandTranslateAmt <= 0)
				{
					rightHandTranslateSpeed = -rightHandTranslateSpeed;
					if (rightHandTranslateAmt <= 0)
					{
						punchCounter++;
						leftHandAnim = true;
						rightHandAnim = false;

					}
				}
			}
		}
		//Check the lap of punch
		if (punchCounter >= 2)
		{
			leftHandAnim = false;
			rightHandAnim = false;
			kickAnim = true;
		}

		legRoteAmt += static_cast<float>(dt) * legRotSpeed;
		if (legRoteAmt >= 20)
		{
			legRoteAmt = 20;
		}
		//Rasie the leg
		if (kickAnim)
		{
			legMovementAmt_ss += static_cast<float>(dt) * (legMovementSpeed_ss * 2 );
			if (legMovementAmt_ss >= 30 || legMovementAmt_ss <= 0)
			{
				//legMovementAmt_ss = 30;
				if (legMovementAmt_ss <= 0)
				{
					legMovementSpeed_ss = 0;
				}
				legMovementSpeed_ss = -legMovementSpeed_ss;	
			}

			bodyTransAmt += static_cast<float>(dt) * (bodyTransSped * 3.1f);
			if (bodyTransAmt >= 10 || bodyTransAmt <= 0)
			{
				bodyTransSped = -bodyTransSped;
				if (bodyTransAmt <= 0)
				{
					bodyTransSped = 0;
				}
			}
			kickAnim = false;
		}	

		//End

		break;
	case ANIM_DEFAULT:
		setDefaultValue();
		
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

	{
		// Load view matrix stack and set it with camera position, target position and up direction
		viewStack.LoadIdentity();
		viewStack.LookAt(camera.position.x, camera.position.y, camera.position.z, camera.target.x, camera.target.y, camera.target.z, camera.up.x, camera.up.y, camera.up.z);

		// Load identity matrix into the model stack
		modelStack.LoadIdentity();

		// Calculate the light position in camera space
		glm::vec3 lightPosition_cameraspace = viewStack.Top() * glm::vec4(light[0].position, 0);
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, glm::value_ptr(lightPosition_cameraspace));

		modelStack.PushMatrix();
		// Render objects
		RenderMesh(meshList[GEO_AXES], false);
		modelStack.PopMatrix();
	}
	
	// Render light
	{
		modelStack.PushMatrix();
		modelStack.Translate(light[0].position.x, light[0].position.y + 4, light[0].position.z);
		modelStack.Scale(0.1f, 0.1f, 0.1f);
		meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
		meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
		meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
		meshList[GEO_SPHERE]->material.kShininess = 1.0f;
		RenderMesh(meshList[GEO_SPHERE], enableLight);
		modelStack.PopMatrix();
	}

	//Whole upper body
	{
		//Render of pelvis
		modelStack.PushMatrix();

		//Rotation of the whole boday
		if (currAnim == ANIM_SWIMMING)
		{
			modelStack.Rotate(100, 1, 0, 0);
			
		}
		else if (currAnim == ANIM_IDLE)
		{
			modelStack.Translate(0, bodyMovementAmt_idle, 0);
		}
		else if (currAnim == ANIM_SUMMERSAULT)
		{
			modelStack.Translate(0, bodyMovementAmt_ss, bodyTransAmt);
			modelStack.Rotate(bodyRotAmt, 1, 0, 0);
		}
		else if (currAnim == ANIM_COMBO_ATTACK)
		{
			modelStack.Rotate(bodyRotAmt, 0, 1, 0);
			modelStack.Rotate(bodyTransAmt * 3, 0, 0, 1);
			
		}
			
		{
			//Render area of head
			{
				//Render of neck
				modelStack.PushMatrix();
				if (currAnim == ANIM_WAVING)
				{
					modelStack.Rotate(headRotateAmt, 0, 0, 1);
				}
				else if (currAnim == ANIM_SWIMMING)
				{
					modelStack.Rotate(-45, 1, 0, 0);
					modelStack.Rotate(headRotateAmt, 0, 1, 0);
				}
				else if (currAnim == ANIM_SUMMERSAULT)
				{
					modelStack.Rotate(headRotateAmt, 1, 0, 0);
				}
				else if (currAnim == ANIM_COMBO_ATTACK)
				{
					modelStack.Rotate(-bodyRotAmt, 0, 1, 0);
				}
				
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
				modelStack.Scale(upperBodySize, 2.65f, upperBodySize);
				meshList[GEO_TORUS_01]->material.kAmbient = glm::vec3(0.75f, 0.25f, 0.03f);
				meshList[GEO_TORUS_01]->material.kDiffuse = glm::vec3(0.4f, 0.4f, 0.4f);
				meshList[GEO_TORUS_01]->material.kSpecular = glm::vec3(0.6f, 0.6f, 0.6f);
				meshList[GEO_TORUS_01]->material.kShininess = 5.0f;
				RenderMesh(meshList[GEO_TORUS_01], true);
				modelStack.PopMatrix();

			}

			//Render of left arm
			{
				//Left clavicle Joints
				modelStack.PushMatrix();
				modelStack.Translate(-1, 0, 0);
				if (currAnim == ANIM_SWIMMING)
				{
					modelStack.Rotate(-rightHandTranslateAmt, 0, 0, 1);
				}
				else if (currAnim == ANIM_WAVING)
				{
					modelStack.Rotate(lefthandTranslateAmt, 0, 0, 1);
				}
				else if (currAnim == ANIM_IDLE)
				{
					modelStack.Rotate(45, 0, 0, 1);
					modelStack.Rotate(-lefthandTranslateAmt, 0, 1, 0);
				}
				else if (currAnim == ANIM_SUMMERSAULT)
				{
					modelStack.Rotate(handRoteAmt, 0, 0, 1);
					modelStack.Rotate(handRoteAmt, 0, 1, 0);
				}
				else if (currAnim == ANIM_COMBO_ATTACK)
				{
					modelStack.Rotate(bodyRotAmt * 2, 0, 0, 1);
					modelStack.Rotate(lefthandTranslateAmt * 5, 0, 1, 0);
				}


				{
					//Left shoulder
					modelStack.PushMatrix();

					{
						//Left forearm joint
						modelStack.PushMatrix();
						modelStack.Translate(-1.3f, 0, 0);
						if (currAnim == ANIM_COMBO_ATTACK)
						{

							modelStack.Rotate(handRoteAmt * 3.5f, 0, 1, 0);
							modelStack.Rotate(-lefthandTranslateAmt * 7, 0, 1, 0);
						}

						{
							//Left forearm
							modelStack.PushMatrix();

							{
								//Left hand joint
								modelStack.PushMatrix();
								modelStack.Translate(-1.25f, 0, 0);
								modelStack.Scale(jointSize, jointSize, jointSize);
								meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(1, 0.2f, 0);
								meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
								meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
								meshList[GEO_SPHERE]->material.kShininess = 1.0f;
								RenderMesh(meshList[GEO_SPHERE], true);
								modelStack.PopMatrix();
							}

							modelStack.Translate(-.2f, 0, 0);
							modelStack.Scale(0.5f, 0.25f, 0.25f);
							modelStack.Rotate(90, 0, 0, 1);
							meshList[GEO_CYLINDER]->material.kAmbient = glm::vec3(1, 0.2f, 0);
							meshList[GEO_CYLINDER]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
							meshList[GEO_CYLINDER]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
							meshList[GEO_CYLINDER]->material.kShininess = 1.0f;
							RenderMesh(meshList[GEO_CYLINDER], true);
							modelStack.PopMatrix();
						}

						modelStack.Scale(jointSize, jointSize, jointSize);

						meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(1, 0.2f, 0);
						meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
						meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
						meshList[GEO_SPHERE]->material.kShininess = 1.0f;
						RenderMesh(meshList[GEO_SPHERE], true);
						modelStack.PopMatrix();
					}

					modelStack.Translate(-.2f, 0, 0);
					modelStack.Rotate(90, 0, 0, 1);
					modelStack.Scale(0.25f, 0.5f, 0.25f);

					meshList[GEO_CYLINDER]->material.kAmbient = glm::vec3(1, 0.2f, 0);
					meshList[GEO_CYLINDER]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
					meshList[GEO_CYLINDER]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
					meshList[GEO_CYLINDER]->material.kShininess = 1.0f;
					RenderMesh(meshList[GEO_CYLINDER], true);
					modelStack.PopMatrix();
				}

				modelStack.Scale(jointSize, jointSize, jointSize);

				meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(1, 0.2f, 0);
				meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
				meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
				meshList[GEO_SPHERE]->material.kShininess = 1.0f;
				RenderMesh(meshList[GEO_SPHERE], true);
				modelStack.PopMatrix();
			}

			//Render of right arm
			{
				//Right clavicle joints ( 1 )
				modelStack.PushMatrix();
				modelStack.Translate(1, 0, 0);
				if (currAnim == ANIM_SWIMMING)
				{
					modelStack.Rotate(rightHandTranslateAmt, 0, 0, 1);
				}
				else if (currAnim == ANIM_WAVING)
				{
					modelStack.Rotate(rightHandTranslateAmt, 0, 0, 1);
				}
				else if (currAnim == ANIM_IDLE)
				{
					modelStack.Rotate(-45, 0, 0, 1);
					modelStack.Rotate(lefthandTranslateAmt, 0, 1, 0);
				}
				else if (currAnim == ANIM_SUMMERSAULT)
				{
					modelStack.Rotate(-handRoteAmt, 0, 0, 1);
					modelStack.Rotate(-handRoteAmt, 0, 1, 0);
				}
				else if (currAnim == ANIM_COMBO_ATTACK)
				{
					modelStack.Rotate(-bodyRotAmt * 2, 0, 0, 1);
					modelStack.Rotate(-rightHandTranslateAmt * 5, 0, 1, 0);
				}

				{
					//Right shoulder
					modelStack.PushMatrix();

					{
						//Right forearm joint ( 2 )
						modelStack.PushMatrix();

						modelStack.Translate(1.3f, 0, 0);
						if (currAnim == ANIM_WAVING)
						{
							modelStack.Rotate(rightForearmRotAmt, 0, 0, 1);
						}

						if (currAnim == ANIM_COMBO_ATTACK)
						{

							modelStack.Rotate(-handRoteAmt * 3.5f, 0, 1, 0);
							modelStack.Rotate(rightHandTranslateAmt * 7, 0, 1, 0);
						}

						{
							//Right forearm
							modelStack.PushMatrix();

							{
								//Right hand joint ( 3 )
								modelStack.PushMatrix();
								modelStack.Translate(1.25f, 0, 0);
								modelStack.Scale(jointSize, jointSize, jointSize);
								meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(1, 0.2f, 0);
								meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
								meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
								meshList[GEO_SPHERE]->material.kShininess = 1.0f;
								RenderMesh(meshList[GEO_SPHERE], true);
								modelStack.PopMatrix();
							}

							modelStack.Translate(.15f, 0, 0);
							modelStack.Scale(0.5f, 0.25f, 0.25f);
							modelStack.Rotate(-90, 0, 0, 1);

							meshList[GEO_CYLINDER]->material.kAmbient = glm::vec3(1, 0.2f, 0);
							meshList[GEO_CYLINDER]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
							meshList[GEO_CYLINDER]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
							meshList[GEO_CYLINDER]->material.kShininess = 1.0f;
							RenderMesh(meshList[GEO_CYLINDER], true);
							modelStack.PopMatrix();
						}

						modelStack.Scale(jointSize, jointSize, jointSize);

						meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(1, 0.2f, 0);
						meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
						meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
						meshList[GEO_SPHERE]->material.kShininess = 1.0f;
						RenderMesh(meshList[GEO_SPHERE], true);
						modelStack.PopMatrix();
					}

					modelStack.Translate(1.2f, 0, 0);
					modelStack.Rotate(90, 0, 0, 1);
					modelStack.Scale(0.25f, 0.5f, 0.25f);

					meshList[GEO_CYLINDER]->material.kAmbient = glm::vec3(1, 0.2f, 0);
					meshList[GEO_CYLINDER]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
					meshList[GEO_CYLINDER]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
					meshList[GEO_CYLINDER]->material.kShininess = 1.0f;
					RenderMesh(meshList[GEO_CYLINDER], true);
					modelStack.PopMatrix();
				}

				modelStack.Scale(jointSize, jointSize, jointSize);
				meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(1, 0.2f, 0);
				meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
				meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
				meshList[GEO_SPHERE]->material.kShininess = 1.0f;
				RenderMesh(meshList[GEO_SPHERE], true);
				modelStack.PopMatrix();
			}
				
			//Render of left leg
			{
				//Left hip Joints (1)
				modelStack.PushMatrix();
				modelStack.Translate(-1, -2, 0);
				if (currAnim == ANIM_IDLE)
				{
					modelStack.Translate(0, -bodyMovementAmt_idle * 0.5f, 0);
				}
				else if (currAnim == ANIM_SUMMERSAULT)
				{
					modelStack.Rotate(-legMovementAmt_ss, 1, 0, 0);
				}
				else if (currAnim == ANIM_SWIMMING)
				{
					modelStack.Rotate(legRoteAmt, 1, 0, 0);
				}
				else if (currAnim == ANIM_COMBO_ATTACK)
				{
					modelStack.Rotate(-legRoteAmt, 1, 0, 1);
					modelStack.Rotate(-bodyTransAmt * 3, 0, 0, 1);
					
				}
				{
					//Left thigh
					modelStack.PushMatrix();
					modelStack.Translate(0, -1.25f, 0);
					{
						//Left Knee (2)
						modelStack.PushMatrix();
						modelStack.Translate(0, -0.15f, 0);
						if (currAnim == ANIM_SUMMERSAULT)
						{
							modelStack.Rotate(legMovementAmt_ss * 2, 1, 0, 0);
						}
						else if (currAnim == ANIM_COMBO_ATTACK)
						{
							modelStack.Rotate(legRoteAmt, 1, 0, 1);
						}
						{
							//Left calf
							modelStack.PushMatrix();
							modelStack.Translate(0, -1.25f, 0);

							{
								//Left ankle (3)
								modelStack.PushMatrix();
								modelStack.Translate(0, -0.25f, 0);
								if (currAnim == ANIM_SUMMERSAULT)
								{
									modelStack.Rotate(-legMovementAmt_ss / 1.5f, 1, 0, 0);
								}
								{
									//Left Legs
									modelStack.PushMatrix();
									modelStack.Translate(0, -0.45f, 0.1f);
									modelStack.Translate(0, bodyMovementAmt_idle, 0);
									modelStack.Scale(0.55f, 0.15f, 0.55f);
									modelStack.Rotate(35, 0, 1, 0);
									meshList[GEO_CUBE]->material.kAmbient = glm::vec3(1, 0.2f, 0);
									meshList[GEO_CUBE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
									meshList[GEO_CUBE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
									meshList[GEO_CUBE]->material.kShininess = 1.0f;
									RenderMesh(meshList[GEO_CUBE], true);
									modelStack.PopMatrix();
								}
								
								modelStack.Scale(jointSize, jointSize, jointSize);
								meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(1, 0.2f, 0);
								meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
								meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
								meshList[GEO_SPHERE]->material.kShininess = 1.0f;
								RenderMesh(meshList[GEO_SPHERE], true);
								modelStack.PopMatrix();
							}
							
							modelStack.Scale(0.25f, 0.65f, 0.25f);
							meshList[GEO_CYLINDER]->material.kAmbient = glm::vec3(1, 0.2f, 0);
							meshList[GEO_CYLINDER]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
							meshList[GEO_CYLINDER]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
							meshList[GEO_CYLINDER]->material.kShininess = 1.0f;
							RenderMesh(meshList[GEO_CYLINDER], true);
							modelStack.PopMatrix();
						}
						
						modelStack.Scale(jointSize, jointSize, jointSize);
						meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(1, 0.2f, 0);
						meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
						meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
						meshList[GEO_SPHERE]->material.kShininess = 1.0f;
						RenderMesh(meshList[GEO_SPHERE], true);
						modelStack.PopMatrix();

					}
					
					modelStack.Scale(0.25f, 0.65f, 0.25f);
					meshList[GEO_CYLINDER]->material.kAmbient = glm::vec3(1, 0.2f, 0);
					meshList[GEO_CYLINDER]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
					meshList[GEO_CYLINDER]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
					meshList[GEO_CYLINDER]->material.kShininess = 1.0f;
					RenderMesh(meshList[GEO_CYLINDER], true);
					modelStack.PopMatrix();
				}
				
				modelStack.Scale(jointSize, jointSize, jointSize);
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
				modelStack.Translate(1.f, -2, 0);	
				if (currAnim == ANIM_IDLE)
				{
					modelStack.Translate(0, -bodyMovementAmt_idle * 0.5f, 0);
				}
				else if (currAnim == ANIM_SUMMERSAULT)
				{
					modelStack.Rotate(-legMovementAmt_ss, 1, 0, 0);
				}
				else if (currAnim == ANIM_SWIMMING)
				{
					modelStack.Rotate(-legRoteAmt, 1, 0, 0);
				}
				else if (currAnim == ANIM_COMBO_ATTACK)
				{
					modelStack.Rotate(legRoteAmt, 1, 0, 1);
					modelStack.Rotate(-legMovementAmt_ss * 5, 1, 0, 0);
				}

				{
					//Right thigh
					modelStack.PushMatrix();
					modelStack.Translate(0, -1.25f, 0);
					{
						//Right Knee
						modelStack.PushMatrix();
						modelStack.Translate(0, -0.15f, 0);
						if (currAnim == ANIM_SUMMERSAULT)
						{
							modelStack.Rotate(legMovementAmt_ss * 2, 1, 0, 0);
						}
						else if (currAnim == ANIM_COMBO_ATTACK)
						{
							modelStack.Rotate(legMovementAmt_ss * 5, 1, 0, 0);
						}
				
						{
							//Right calf
							modelStack.PushMatrix();
							modelStack.Translate(0, -1.25f, 0);
							{
								//Right ankle
								modelStack.PushMatrix();
								modelStack.Translate(0, -0.25f, 0);
								if (currAnim == ANIM_SUMMERSAULT)
								{
									modelStack.Rotate(-legMovementAmt_ss/1.5f, 1, 0, 0);
								}
								{
									//Right Legs
									modelStack.PushMatrix();
									modelStack.Translate(0, -0.45f, 0.1f);
									modelStack.Translate(0, bodyMovementAmt_idle, 0);
									modelStack.Scale(0.55f, 0.15f, 0.55f);
									modelStack.Rotate(-35, 0, 1, 0);
									meshList[GEO_CUBE]->material.kAmbient = glm::vec3(1, 0.2f, 0);
									meshList[GEO_CUBE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
									meshList[GEO_CUBE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
									meshList[GEO_CUBE]->material.kShininess = 1.0f;
									RenderMesh(meshList[GEO_CUBE], true);
									modelStack.PopMatrix();
								}
								
								modelStack.Scale(jointSize, jointSize, jointSize);
								meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(1, 0.2f, 0);
								meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
								meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
								meshList[GEO_SPHERE]->material.kShininess = 1.0f;
								RenderMesh(meshList[GEO_SPHERE], true);
								modelStack.PopMatrix();
							}
							
							modelStack.Scale(0.25f, 0.65f, 0.25f);
							meshList[GEO_CYLINDER]->material.kAmbient = glm::vec3(1, 0.2f, 0);
							meshList[GEO_CYLINDER]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
							meshList[GEO_CYLINDER]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
							meshList[GEO_CYLINDER]->material.kShininess = 1.0f;
							RenderMesh(meshList[GEO_CYLINDER], true);
							modelStack.PopMatrix();
						}
						
						modelStack.Scale(jointSize, jointSize, jointSize);
						meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(1, 0.2f, 0);
						meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
						meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
						meshList[GEO_SPHERE]->material.kShininess = 1.0f;
						RenderMesh(meshList[GEO_SPHERE], true);
						modelStack.PopMatrix();
					}
					
					modelStack.Scale(0.25f, 0.65f, 0.25f);
					meshList[GEO_CYLINDER]->material.kAmbient = glm::vec3(1, 0.2f, 0);
					meshList[GEO_CYLINDER]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
					meshList[GEO_CYLINDER]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
					meshList[GEO_CYLINDER]->material.kShininess = 1.0f;
					RenderMesh(meshList[GEO_CYLINDER], true);
					modelStack.PopMatrix();
				}
				
				modelStack.Scale(jointSize, jointSize, jointSize);
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
	if (KeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_0))
	{
		// Toggle light on or off
		std::cout << "Light !" << enableLight <<'\n';
		enableLight = !enableLight;
	}

	//Start idle animation
	if (KeyboardController::GetInstance()->IsKeyPressed('Z'))
	{
		setDefaultValue();
		currAnim = ANIM_IDLE;
		std::cout <<"IDLING" << '\n';
	}
	if (KeyboardController::GetInstance()->IsKeyPressed('X'))
	{
		setDefaultValue();
		currAnim = ANIM_WAVING;
		std::cout << "Waving !" << '\n';
	}
	if (KeyboardController::GetInstance()->IsKeyPressed('C'))
	{
		setDefaultValue();
		currAnim = ANIM_SWIMMING;
		std::cout << "Swimming !" << '\n';
	}
	if (KeyboardController::GetInstance()->IsKeyPressed('V'))
	{
		setDefaultValue();
		currAnim = ANIM_SUMMERSAULT;
		std::cout << "Summersaulting !" << '\n';
	}
	if (KeyboardController::GetInstance()->IsKeyPressed('B'))
	{
		setDefaultValue();
		currAnim = ANIM_COMBO_ATTACK;
		std::cout << "Combo attacking !" << '\n';
	}
	if (KeyboardController::GetInstance()->IsKeyPressed('R'))
	{
		currAnim = ANIM_DEFAULT;
		std::cout << "Default Pos !" << '\n';
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

