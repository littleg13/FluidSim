// BenchChair.c++

#include "BenchChair.h"

BenchChair::BenchChair(ShaderIF* sIF, PhongMaterial matIn) : SceneElement(sIF), chairColor(matIn)
{
	initModelGeometry();
	calculateBoundingBox();
}

BenchChair::~BenchChair()
{
	for(int i=0;i<n_prisms;i++){
		delete prisms[i];
	}
	delete[] prisms;
}

void BenchChair::calculateBoundingBox(){
	double xyzTemp[6];
	prisms[0]->getMCBoundingBox(xyz);
	xyz[0] += m_transform[0].elementAt(0, 3);
	xyz[1] += m_transform[0].elementAt(0, 3);
	xyz[2] += m_transform[0].elementAt(1, 3);
	xyz[3] += m_transform[0].elementAt(1, 3);
	xyz[4] += m_transform[0].elementAt(2, 3);
	xyz[5] += m_transform[0].elementAt(2, 3);
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
void BenchChair::getMCBoundingBox(double* xyzLimits) const
{
	xyzLimits[0] = xyz[0]; // xmin  Give real values!
	xyzLimits[1] = xyz[1]; // xmax         |
	xyzLimits[2] = xyz[2]; // ymin         |
	xyzLimits[3] = xyz[3]; // ymax         |
	xyzLimits[4] = xyz[4]; // zmin         |
	xyzLimits[5] = xyz[5]; // zmax        \_/
}


void BenchChair::initModelGeometry(){
	prisms = new Prism*[n_prisms];
	float translate[n_prisms][3] = {
	//	 x, y, z
		{0, 2.125, 0.5},
		{0, 2.125, 1.25},
		{0, 2.125, 2},
		{0, 2.25, 0.125},
		{0, 3.0, 0.125},
		{0, 3.75, 0.125},
		{0, 4.5, 0.125},
	};
	float scale[n_prisms][3] = {
	//	 x, y, z
		{7.5, 0.3, 0.5},
		{7.5, 0.3, 0.5},
		{7.5, 0.3, 0.5},
		{7.5, 0.5, 0.3},
		{7.5, 0.5, 0.3},
		{7.5, 0.5, 0.3},
		{7.5, 0.5, 0.3},
	};
	for(int i=0;i<n_prisms;i++){
		prisms[i] = new Prism(shaderIF, chairColor, scale[i], true, 0.05);
		m_transform[i] = cryph::Matrix4x4::translation(translate[i]);
	}
}

void BenchChair::render()
{
	GLint pgm;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pgm);

	// draw the triangles using our vertex and fragment shaders
	glUseProgram(shaderIF->getShaderPgmID());

	renderBenchChair(cryph::Matrix4x4());

	// 5. Reestablish previous shader program
	glUseProgram(pgm);
	// ...
}

void BenchChair::renderBenchChair(cryph::Matrix4x4 M)
{
	cryph::Matrix4x4 m_local;
	for(int i=0;i<n_prisms;i++){
		m_local = M * m_transform[i];
		prisms[i]->renderPrism(m_local);
		
	}
}
