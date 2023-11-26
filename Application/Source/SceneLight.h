#ifndef SCNEN_LIGHT
#define SCNEN_LIGHT

#include "Scene.h"
#include "Mesh.h"
#include "AltAzCamera.h"
#include "MatrixStack.h"
#include "SceneLight.h"
#include "Light.h"

class SceneLight : public Scene
{
public:
	enum GEOMETRY_TYPE
	{
		GEO_AXES,

		GEO_SPHERE,
		GEO_TORUS,
		GEO_QUAD,
		GEO_TORUS_01,
		GEO_CYLINDER,
		GEO_CUBE,

		NUM_GEOMETRY,
	};

	enum UNIFORM_TYPE
	{
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,
		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHTENABLED,
		U_TOTAL,
	};

	enum ANIMATION
	{
		ANIM_DEFAULT,
		ANIM_IDLE,
		ANIM_WAVING,
		ANIM_SWIMMING,
		ANIM_SUMMERSAULT,
		ANIM_COMBO_ATTACK,

		NUM_ANIM
	};

	SceneLight();
	~SceneLight();

	void setDefaultValue();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

private:
	void RenderMesh(Mesh* mesh, bool enableLight);
	
	AltAzCamera camera;

	void HandleKeyPress();

	unsigned m_vertexArrayID;
	Mesh* meshList[NUM_GEOMETRY];

	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];

	// other variables

	MatrixStack modelStack, viewStack, projectionStack;

	int projType = 1; // fix to 0 for orthographic, 1 for projection
	
	float earthRotation;
	float sunRotation;

	Light light[1];
	bool enableLight;
	// Keep track of current animation

	ANIMATION currAnim;
	//Common
	float jointSize;

	//Head regions
	float headRotateAmt;
	float headRotateSpeed;
	float eyeSize;

	//Chest
	float upperBodySize;

	//Left hand
	float lefthandTranslateAmt;
	float leftHandTranslateSpeed;

	float lefthandRotSpeed;

	//Right hand
	float rightHandTranslateAmt;
	float rightHandTranslateSpeed;

	float rightForearmRotAmt;

	float rightHandRotateSpeed;
	float rightForearmRotSpeed;

	//Left leg
	float leftlegRotateAmt;
	float leftlegRotSpeed;
	//Right leg
	float rightlegRoteAmt;
	float rightlegRotespeed;

	//Common use var
	float legRoteAmt;
	float legRotSpeed;

	float bodyMovementAmt_idle;
	float bodyMovementSpeed_idle;

	float bodyMovementAmt_ss;
	float bodyMovementSpeed_ss;

	float legMovementAmt_ss;
	float legMovementSpeed_ss;

	float handRoteAmt;
	float handRoteSpeed;

	float bodyRotAmt;
	float bodyRotSpeed;
	float bodyRotSpeed_ca;

	float bodyTransAmt;
	float bodyTransSped;

	bool finishAnim_SS;

	bool rightHandAnim;
	bool leftHandAnim;
	bool kickAnim;

	int punchCounter;


};

#endif