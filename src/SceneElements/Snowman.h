// Snowman.h

#ifndef SNOWMAN_H
#define SNOWMAN_H

#include <cstdlib>

#include "SceneElement.h"
#include "BasicShapeRenderer.h"

class Snowman : public SceneElement
{
	typedef float vec3[3];
	
public:
	// As before: you will likely want to add parameters to the constructor
	Snowman(ShaderIF* sIF, cryph::AffVector pos, float pScale);
	virtual ~Snowman();

	// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
	void getMCBoundingBox(double* xyzLimitsF) const;
	void render();
	void renderSnowman(cryph::Matrix4x4 M);
private:
	double xyz[6];
	cryph::AffVector position;
	PhongMaterial solidSnow = PhongMaterial(0.45, 0.45, 0.5, 0.55, 0.55, 0.6, 0.8, 0.8, 1, 1, 1.0);
	PhongMaterial benchBrown = PhongMaterial(0.6, 0.5, 0.2, 0.2, 0.9, 0.2, 5, 1.0);
	PhongMaterial carrotCrayon = PhongMaterial(0.8, 0.6, 0.1, 0.4, 0.7, 0.7, 5, 1.0);
	PhongMaterial blackBall = PhongMaterial(0.2, 0.2, 0.2, 0.2, 0.7, 0.1, 1, 1.0);
	int static const n_components = 8;
	float scale;
	PhongMaterial materials[n_components];
	BasicShape* components[n_components];
	BasicShapeRenderer* renderers[n_components];
	cryph::Matrix4x4* m_transform;
	static void prepareForFace(void* caller, int faceIndex);
	void calculateBoundingBox();
	void initModelGeometry();
};

#endif
