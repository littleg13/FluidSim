// Prism.h

#ifndef PRISM_H
#define PRISM_H

#include "SceneElement.h"

class Prism : public SceneElement
{
typedef float vec3[3];
	
public:
	// As before: you will likely want to add parameters to the constructor
	Prism(ShaderIF* sIF, PhongMaterial& matlIn, vec3 dim, bool closed, float cornerRadius);
	virtual ~Prism();

	// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
	void getMCBoundingBox(double* xyzLimitsF) const;
	void render();
	void renderPrism(cryph::Matrix4x4 M);
private:
	GLuint vbo[2], vao[1], ebo[1];
	PhongMaterial matl;
	float xmin, xmax, ymin, ymax, zmin, zmax;
	vec3 dimensions;
	int vboShift = 0;
	int indexShift = 0;
	int n_faces;
	int n_vertices = 536;
	vec3* vertices = new vec3[n_vertices];
	GLuint indicesList[732][3];
	vec3* normals = new vec3[n_vertices]; 
	float radius;
	void calculateBoundingBox();
	void initModelGeometry();
	void generateEdges(int anglePrecision);
	void generateCorners(int anglePrecision);
	void addQuad(int v00, int v01, int v10, int v11);
};

#endif
