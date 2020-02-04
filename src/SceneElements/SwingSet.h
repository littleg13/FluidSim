// SwingSet.h

#ifndef SWINGSET_H
#define SWINGSET_H

#include <cstdlib>

#include "SceneElement.h"
#include "Prism.h"
#include "Cylinder.h"

class SwingSet : public SceneElement
{
	typedef float vec3[3];
	
public:
	// As before: you will likely want to add parameters to the constructor
	SwingSet(ShaderIF* sIF, cryph::AffVector pos, float pScale);
	virtual ~SwingSet();

	// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
	void getMCBoundingBox(double* xyzLimitsF) const;
	void render();
	void renderSwingSet(cryph::Matrix4x4 M);
private:
	double xyz[6];
	cryph::AffVector position;
	GLuint texID[1];
	PhongMaterial metalMaroon = PhongMaterial(0.3984, 0.0, 0.199, 0.2, 0.7, 1.2, 10, 1);
	PhongMaterial mushyMetal = PhongMaterial(0.6, 0.6, 0.6, 0.2, 0.8, 1.9, 25.0, 1.0);
	PhongMaterial benchBrown = PhongMaterial(0.6, 0.5, 0.2, 0.2, 0.9, 0.2, 5, 1.0);
	int static const n_components = 13;
	float scale;
	ModelView** components;
	Prism* leftBaseFront;
	Prism* leftBaseBack;
	Prism* leftBaseTop;
	Prism* rightBaseFront;
	Prism* rightBaseBack;
	Prism* rightBaseTop;
	Prism* crossbar;
	Cylinder* leftChain1;
	Cylinder* rightChain1;
	Cylinder* leftChain2;
	Cylinder* rightChain2;
	Prism* swing1;
	Prism* swing2;
	cryph::Matrix4x4* m_transform;

	void calculateBoundingBox();
	void initModelGeometry();
};

#endif
