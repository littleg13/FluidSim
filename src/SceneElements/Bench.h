// Bench.h

#ifndef BENCH_H
#define BENCH_H

#include "SceneElement.h"
#include "BenchChair.h"
#include "BenchArm.h"

class Bench : public SceneElement
{
	typedef float vec3[3];
	
public:
	// As before: you will likely want to add parameters to the constructor
	Bench(ShaderIF* sIF, cryph::AffVector pos, float r, float s);
	virtual ~Bench();

	// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
	void getMCBoundingBox(double* xyzLimitsF) const;
	void render();
	void renderBench(cryph::Matrix4x4 M);
private:
	GLuint vbo[1], vao[1];
	double xyz[6];
	cryph::AffVector position;
	float rotation, scale;
	BenchChair* chair;
	BenchArm* leftArm;
	BenchArm* rightArm;
	ModelView** components = new ModelView*[3];
	cryph::Matrix4x4 m_transform[3];
	PhongMaterial benchBrown = PhongMaterial(0.5, 0.4, 0.25, 0.2, 0.9, 0.5, 5, 1.0);

	void calculateBoundingBox();
	void initModelGeometry();
};

#endif
