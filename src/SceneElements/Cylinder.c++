// Cylinder.c++

#include "Cylinder.h"

Cylinder::Cylinder(ShaderIF* sIF, PhongMaterial& matlIn, float r, float h) : SceneElement(sIF), matl(matlIn), radius(r), height(h)
{
	useTexture = false;
	initModelGeometry();
	calculateBoundingBox();
}
Cylinder::Cylinder(ShaderIF* sIF, PhongMaterial& matlIn, float r, float h, std::string textureSource) : SceneElement(sIF), matl(matlIn), texSource(textureSource), radius(r), height(h)
{
	useTexture = true;
	initModelGeometry();
	calculateBoundingBox();
}

Cylinder::~Cylinder()
{
	delete[] vertices;
	delete[] normals;
	delete[] texCoords;
	glDeleteBuffers(3, vbo);
	glDeleteVertexArrays(1, vao);
}

void Cylinder::calculateBoundingBox(){
	xmin = zmin = -radius;
	xmax = zmax = radius;
	ymin = 0;
	ymax = height;
}

// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
void Cylinder::getMCBoundingBox(double* xyzLimits) const
{
	xyzLimits[0] = xmin; // xmin  Give real values!
	xyzLimits[1] = xmax; // xmax         |
	xyzLimits[2] = ymin; // ymin         |
	xyzLimits[3] = ymax; // ymax         |
	xyzLimits[4] = zmin; // zmin         |
	xyzLimits[5] = zmax; // zmax        \_/
}

void Cylinder::initModelGeometry(){
	int vboShift = 0;
	for(int theta=0;theta<=360;theta+=360/verticesAround){
		cryph::AffPoint p = radius * cryph::AffVector::xu;
		p = cryph::Matrix4x4::yRotationDegrees(theta) * p;
		cryph::AffVector normal = p - cryph::AffPoint();
		p.aCoords(vertices[vboShift]);
		normal.vComponents(normals[vboShift]);
		texCoords[vboShift][0] = theta / 180.0;
		texCoords[vboShift][1] = 0;
		vboShift++;
		(p + cryph::AffVector::yu * height).aCoords(vertices[vboShift]);
		normal.vComponents(normals[vboShift]);
		texCoords[vboShift][0] = theta / 180.0;
		texCoords[vboShift][1] = tiling;
		vboShift++;
	}


	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(3, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	int numBytes = sizeof(float) * 3 * n_vertices;
	glBufferData(GL_ARRAY_BUFFER, numBytes, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(shaderIF->pvaLoc("mcPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shaderIF->pvaLoc("mcPosition"));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, numBytes, normals, GL_STATIC_DRAW);
	glVertexAttribPointer(shaderIF->pvaLoc("mcNormal"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shaderIF->pvaLoc("mcNormal"));

	numBytes = sizeof(float) * 2 * n_vertices;
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, numBytes, texCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(shaderIF->pvaLoc("texCoords"), 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shaderIF->pvaLoc("texCoords"));

	if(useTexture){
		glActiveTexture(GL_TEXTURE0);
		texID[0] = readTextureImage(texSource);
	}
}


void Cylinder::render()
{
	GLint pgm;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pgm);

	// draw the triangles using our vertex and fragment shaders
	glUseProgram(shaderIF->getShaderPgmID());

	// 2. Establish "mc_ec" and "ec_lds" matrices
	// ...
	renderCylinder(cryph::Matrix4x4());

	// 5. Reestablish previous shader program
	glUseProgram(pgm);
	// ...
}

void Cylinder::renderCylinder(cryph::Matrix4x4 M)
{
	establishView(M);
	if(useTexture)
		establishTexture(texID[0], true, GL_MIRRORED_REPEAT);
	establishMaterial(matl);
	establishLightingEnvironment();

	glBindVertexArray(vao[0]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, n_vertices);
}
