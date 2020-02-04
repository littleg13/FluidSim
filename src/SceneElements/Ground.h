// Ground.h

#ifndef GROUND_H
#define GROUND_H

#include "SceneElement.h"
#include "PNGImageReader.h"
#include <math.h>

class Ground : public SceneElement
{
	typedef float vec3[3];
	typedef float vec2[2];
public:
	// As before: you will likely want to add parameters to the constructor
	Ground(ShaderIF* sIF);
	virtual ~Ground();

	// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
	void getMCBoundingBox(double* xyzLimitsF) const;
	void render();
	void renderGround();
private:
	const float size = 10;
	int precision = 25;
	GLuint vbo[3], vao[1], texID[1];
	float xmin, xmax, ymin, ymax, zmin, zmax;
	const int texTiling = 10;
	vec3* vertices;
	vec2* texCoords;
	vec3* normals;
	int n_vertices;
	PhongMaterial grassGreen = PhongMaterial(0.242, 0.605, 0.222, 0.2, 0.7, 0.3, 10, 1.0);
	int pixelIncrement;
	PNGImageReader* noiseImage;
	void calculateBoundingBox();
	void initModelGeometry();
};

#endif
