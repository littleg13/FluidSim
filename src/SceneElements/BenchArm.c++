// BenchArm.c++

#include "BenchArm.h"

BenchArm::BenchArm(ShaderIF* sIF, PhongMaterial matIn) : SceneElement(sIF), armColor(matIn)
{
	initModelGeometry();
	calculateBoundingBox();
}

BenchArm::~BenchArm()
{
	for(int i=0;i<n_prisms;i++){
		delete prisms[i];
	}
	delete[] prisms;
}

void BenchArm::calculateBoundingBox(){
	double xyzTemp[6];
	prisms[0]->getMCBoundingBox(xyz);
	xyz[0] += m_transform[0].elementAt(0, 3);
	xyz[1] += m_transform[0].elementAt(1, 3);
	xyz[2] += m_transform[0].elementAt(2, 3);
	for(int i=1;i<n_prisms;i++){
		prisms[i]->getMCBoundingBox(xyzTemp);
		for(int j=0;j<6;j+=2){
			if(xyz[j] > xyzTemp[j] + m_transform[i].elementAt(j/2, 3))
				xyz[j] = xyzTemp[j] + m_transform[i].elementAt(j/2, 3);
			if(xyz[j+1] < xyzTemp[j+1] + m_transform[i].elementAt(j/2, 3))
				xyz[j+1] = xyzTemp[j+1] + m_transform[i].elementAt(j/2, 3);
		}
	}
}

// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
void BenchArm::getMCBoundingBox(double* xyzLimits) const
{
	xyzLimits[0] = xyz[0]; // xmin  Give real values!
	xyzLimits[1] = xyz[1]; // xmax         |
	xyzLimits[2] = xyz[2]; // ymin         |
	xyzLimits[3] = xyz[3]; // ymax         |
	xyzLimits[4] = xyz[4]; // zmin         |
	xyzLimits[5] = xyz[5]; // zmax        \_/
}


void BenchArm::initModelGeometry(){
	prisms = new Prism*[n_prisms];
	float translate[n_prisms][3] = {
	//	 x, y, z
		{0, 0, 0},
		{0, 0, 2.0},
		{0, 2.0, 0},
		{0, 2.5, 0},
		{-0.1, 3.0, 0.5},
		{0, 2.5, 2.0},
	};
	float scale[n_prisms][3] = {
	//	 x, y, z
		{0.5, 2.0, 0.5},
		{0.5, 2.0, 0.5},
		{0.5, 0.5, 2.75},
		{0.5, 2.5, 0.5},
		{0.7, 0.3, 2.25},
		{0.5, 0.5, 0.5},
	};
	for(int i=0;i<n_prisms;i++){
		prisms[i] = new Prism(shaderIF, armColor, scale[i], true, 0.05);
		m_transform[i] = cryph::Matrix4x4::translation(translate[i]);
	}
}

void BenchArm::render()
{
	GLint pgm;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pgm);

	// draw the triangles using our vertex and fragment shaders
	glUseProgram(shaderIF->getShaderPgmID());

	renderBenchArm(cryph::Matrix4x4());

	// 5. Reestablish previous shader program
	glUseProgram(pgm);
	// ...
}

void BenchArm::renderBenchArm(cryph::Matrix4x4 M)
{
	cryph::Matrix4x4 m_local;
	for(int i=0;i<n_prisms;i++){
		m_local = M * m_transform[i];
		prisms[i]->renderPrism(m_local);
		
	}
}
