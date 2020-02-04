// Cylinder.h

#ifndef Cylinder_H
#define Cylinder_H

#include "SceneElement.h"
#include "GLFWController.h"
#include <math.h>

class Cylinder : public SceneElement
{
typedef float vec3[3];
typedef float vec2[2];
	
public:
	// As before: you will likely want to add parameters to the constructor
	Cylinder(ShaderIF* sIF, PhongMaterial& matlIn, float radius, float height);
	Cylinder(ShaderIF* sIF, PhongMaterial& matlIn, float r, float h, std::string textureSource);
	virtual ~Cylinder();

	// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
	void getMCBoundingBox(double* xyzLimitsF) const;
	void render();
	void renderCylinder(cryph::Matrix4x4 M);
private:
	PhongMaterial matl;
	GLuint vbo[3], vao[1], texID[1];
	float xmin, xmax, ymin, ymax, zmin, zmax;
	const int verticesAround = 36;
	int n_vertices = (verticesAround + 1) * 2;
	vec3* vertices = new vec3[n_vertices];
	vec3* normals = new vec3[n_vertices];
	vec2* texCoords = new vec2[n_vertices];
	float radius, height;
	bool useTexture = false;
	std::string texSource = "";
	int tiling = 3;
	void calculateBoundingBox();
	void initModelGeometry();
};

#endif
