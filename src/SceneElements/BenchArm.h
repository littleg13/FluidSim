// BenchArm.h

#ifndef BENCHARM_H
#define BENCHARM_H

#include "SceneElement.h"
#include "Prism.h"

class BenchArm : public SceneElement
{
	typedef float vec3[3];
	
public:
	// As before: you will likely want to add parameters to the constructor
	BenchArm(ShaderIF* sIF, PhongMaterial matIn);
	virtual ~BenchArm();

	// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
	void getMCBoundingBox(double* xyzLimitsF) const;
	void render();
	void renderBenchArm(cryph::Matrix4x4 M);
private:
	double xyz[6];
	PhongMaterial armColor;
	int static const n_prisms = 6;
	Prism** prisms;
	cryph::Matrix4x4 m_transform[6];

	void calculateBoundingBox();
	void initModelGeometry();
};

#endif
