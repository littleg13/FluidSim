// BenchChair.h

#ifndef BENCHCHAIR_H
#define BENCHCHAIR_H

#include "SceneElement.h"
#include "Prism.h"

class BenchChair : public SceneElement
{
	typedef float vec3[3];
	
public:
	// As before: you will likely want to add parameters to the constructor
	BenchChair(ShaderIF* sIF, PhongMaterial matIn);
	virtual ~BenchChair();

	// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
	void getMCBoundingBox(double* xyzLimitsF) const;
	void render();
	void renderBenchChair(cryph::Matrix4x4 M);
private:
	double xyz[6];
	PhongMaterial chairColor;
	int static const n_prisms = 7;
	Prism** prisms;
	cryph::Matrix4x4 m_transform[7];

	void calculateBoundingBox();
	void initModelGeometry();
};

#endif
