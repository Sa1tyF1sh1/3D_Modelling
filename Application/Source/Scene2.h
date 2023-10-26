#ifndef SCENE_2_H
#define SCENE_2_H

#include "Scene.h"
#include "Mesh.h"
#include "AltAzCamera.h"

class Scene2 : public Scene
{
public:
	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_QUAD,
		GEO_CIRCLE,
		GEO_SPHERE,
		NUM_GEOMETRY,
	};

	enum SCENE
	{
		U_MVP = 0,
		U_TOTAL,
	};

	Scene2();
	~Scene2();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

private:
	AltAzCamera camera;

	void HandleKeyPress();

	unsigned m_vertexArrayID;
	Mesh* meshList[NUM_GEOMETRY];

	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];

	// other variables
	
	int projType = 1; // fix to 0 for orthographic, 1 for projection
};

#endif