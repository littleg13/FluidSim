// Path.h

#ifndef PATH_H
#define PATH_H

#include <cstdlib>

#include "SceneElement.h"
#include "Prism.h"

class Path : public SceneElement
{
	typedef float vec3[3];
	
public:
	// As before: you will likely want to add parameters to the constructor
	Path(ShaderIF* sIF, cryph::AffVector pos, vec3 dim, float rot, float scale);
	virtual ~Path();

	// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
	void getMCBoundingBox(double* xyzLimitsF) const;
	void render();
	void renderPath(cryph::Matrix4x4 M);
private:
	double xyz[6];
	cryph::AffVector position;
	PhongMaterial palePath = PhongMaterial(0.6, 0.546875, 0.55078125, 0.3, 0.8, 0.1, 1, 1);
	vec3 dimensions;
	int n_prisms;
	float rotation, scale;
	Prism** prisms;
	cryph::Matrix4x4* m_transform;

	void calculateBoundingBox();
	void initModelGeometry();
};

#endif
