// Bench.c++

#include "Bench.h"

Bench::Bench(ShaderIF* sIF, cryph::AffVector pos, float r, float s) : SceneElement(sIF), position(pos), rotation(r), scale(s)
{
	initModelGeometry();
	calculateBoundingBox();
}

Bench::~Bench()
{
	for(int i=0;i<3;i++){
		delete components[i];
	}
	delete[] components;
}

void Bench::calculateBoundingBox(){
	double xyzTemp[6];
	components[0]->getMCBoundingBox(xyz);
	xyz[0] = xyz[0] * m_transform[0].elementAt(0, 0) + m_transform[0].elementAt(0, 3);
	xyz[1] = xyz[1] * m_transform[0].elementAt(0, 0) + m_transform[0].elementAt(0, 3);
	xyz[2] = xyz[2] * m_transform[0].elementAt(1, 1) + m_transform[0].elementAt(1, 3);
	xyz[3] = xyz[3] * m_transform[0].elementAt(1, 1) + m_transform[0].elementAt(1, 3);
	xyz[4] = xyz[4] * m_transform[0].elementAt(2, 2) + m_transform[0].elementAt(2, 3);
	xyz[5] = xyz[5] * m_transform[0].elementAt(2, 2) + m_transform[0].elementAt(2, 3);
	for(int i=1;i<3;i++){
		components[i]->getMCBoundingBox(xyzTemp);
		for(int j=0;j<6;j+=2){
			if(xyz[j] > xyzTemp[j] * m_transform[i].elementAt(j/2, j/2) + m_transform[i].elementAt(j/2, 3))
				xyz[j] = xyzTemp[j] * m_transform[i].elementAt(j/2, j/2) + m_transform[i].elementAt(j/2, 3);
			if(xyz[j+1] < xyzTemp[j+1] * m_transform[i].elementAt(j/2, j/2) + m_transform[i].elementAt(j/2, 3))
				xyz[j+1] = xyzTemp[j+1] * m_transform[i].elementAt(j/2, j/2) + m_transform[i].elementAt(j/2, 3);
		}
	}
	
}

// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
void Bench::getMCBoundingBox(double* xyzLimits) const
{
	xyzLimits[0] = xyz[0]; // xmin  Give real values!
	xyzLimits[1] = xyz[1]; // xmax         |
	xyzLimits[2] = xyz[2]; // ymin         |
	xyzLimits[3] = xyz[3]; // ymax         |
	xyzLimits[4] = xyz[4]; // zmin         |
	xyzLimits[5] = xyz[5]; // zmax        \_/
}


void Bench::initModelGeometry(){
	cryph::AffVector translate[3] = {
	//	 x, y, z
		cryph::AffVector(0, 0, 0),
		cryph::AffVector(8.0, 0, 0),
		cryph::AffVector(0.5, 0, 0),
	};
	for(int i=0;i<3;i++){
		m_transform[i] = cryph::Matrix4x4::generalRotationDegrees(cryph::AffPoint() + position, cryph::AffVector::yu, rotation) * cryph::Matrix4x4(double(scale) * cryph::Matrix3x3(), translate[i] * double(scale) + position);
	}
	leftArm = new BenchArm(shaderIF, benchBrown);
	components[0] = leftArm;
	rightArm = new BenchArm(shaderIF, benchBrown);
	components[1] = rightArm;
	chair = new BenchChair(shaderIF, benchBrown);
	components[2] = chair;
}

void Bench::render()
{
	GLint pgm;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pgm);

	// draw the triangles using our vertex and fragment shaders
	glUseProgram(shaderIF->getShaderPgmID());

	renderBench(cryph::Matrix4x4());

	// 5. Reestablish previous shader program
	glUseProgram(pgm);
	// ...
}

void Bench::renderBench(cryph::Matrix4x4 M)
{
	glUniform1f(shaderIF->ppuLoc("snowScale"), scale);
	cryph::Matrix4x4 m_local;
	m_local = M * m_transform[0];
	leftArm->renderBenchArm(m_local);
	m_local = M * m_transform[1];
	rightArm->renderBenchArm(m_local);
	m_local = M * m_transform[2];
	chair->renderBenchChair(m_local);
}
