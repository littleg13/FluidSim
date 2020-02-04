// LampPost.h

#ifndef LAMPPOST_H
#define LAMPPOST_H

#include <cstdlib>

#include "SceneElement.h"
#include "Prism.h"
#include "Cylinder.h"

class LampPost : public SceneElement
{
	typedef float vec3[3];
	
public:
	// As before: you will likely want to add parameters to the constructor
	LampPost(ShaderIF* sIF, cryph::AffVector pos, float pScale);
	virtual ~LampPost();

	// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
	void getMCBoundingBox(double* xyzLimitsF) const;
	void render();
	void renderLampPost(cryph::Matrix4x4 M);
private:
	double xyz[6];
	cryph::AffVector position;
	PhongMaterial loomingGreyLampPost = PhongMaterial(0.125, 0.125, 0.125, 0.3, 0.5, 1.2, 25, 1);
	PhongMaterial washedWhite = PhongMaterial(0.8, 0.8, 0.7, 0.4, 0.8, 0.9, 15, 0.5);
	int static const n_components = 4;
	float scale;
	ModelView** components;
	Prism* base;
	Cylinder* pole;
	Prism* lamp;
	Prism* lid;
	cryph::Matrix4x4* m_transform;

	void calculateBoundingBox();
	void initModelGeometry();
};

#endif
