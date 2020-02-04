// Path.c++

#include "Path.h"

Path::Path(ShaderIF* sIF, cryph::AffVector pos, vec3 dim, float rot, float pScale) : SceneElement(sIF), position(pos), rotation(rot), scale(pScale)
{
	for(int i=0;i<3;i++){
		dimensions[i] = dim[i];
	}
	initModelGeometry();
	calculateBoundingBox();
}

Path::~Path()
{
}

void Path::calculateBoundingBox(){
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
void Path::getMCBoundingBox(double* xyzLimits) const
{
	xyzLimits[0] = xyz[0]; // xmin  Give real values!
	xyzLimits[1] = xyz[1]; // xmax         |
	xyzLimits[2] = xyz[2]; // ymin         |
	xyzLimits[3] = xyz[3]; // ymax         |
	xyzLimits[4] = xyz[4]; // zmin         |
	xyzLimits[5] = xyz[5]; // zmax        \_/
}


void Path::initModelGeometry(){
	vec3 brickSize = {0.8, 0.35, 0.358};
	for(int i=0;i<3;i++){
		brickSize[i] *= scale;
	}
	int numX = int(dimensions[0] / brickSize[0]);
	int numZ = int(dimensions[2] / brickSize[2]);
	n_prisms = numX * numZ + numZ/2;
	prisms = new Prism*[n_prisms];
	m_transform = new cryph::Matrix4x4[n_prisms];
	cryph::AffVector* translate = new cryph::AffVector[n_prisms];
	vec3* size = new vec3[n_prisms];
	int count = 0;
	for(int i=0;i<numZ;i++){
		float offset = 0;
		if(i%2 == 1){
			offset = brickSize[0] / 2.0;
			translate[count] = cryph::AffVector(0, 0 , i * brickSize[2]) + position;
			size[count][0] = brickSize[0] / 2.0;
			size[count][1] = brickSize[1];
			size[count][2] = brickSize[2]; 
			count++;
		}
		for(int j=0;j<numX;j++){
			translate[count] = cryph::AffVector(j * brickSize[0] + offset, 0 , i * brickSize[2]) + position;
			size[count][0] = brickSize[0];
			size[count][1] = brickSize[1];
			size[count][2] = brickSize[2];
			if(offset != 0 && j == numX-1)
				size[count][0] = brickSize[0] / 2.0;
			count++;
		}
	}
	for(int i=0;i<n_prisms;i++){
		float variance = 1.0 * rand() / (float)RAND_MAX + 0.25;
		prisms[i] = new Prism(shaderIF, palePath, size[i], true, variance * 0.05 * scale);
		m_transform[i] = cryph::Matrix4x4::generalRotationDegrees(cryph::AffPoint() + position, cryph::AffVector::yu, rotation) * cryph::Matrix4x4::translation(translate[i]);
	}
	delete[] translate;
	delete[] size;
}

void Path::render()
{
	GLint pgm;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pgm);

	// draw the triangles using our vertex and fragment shaders
	glUseProgram(shaderIF->getShaderPgmID());

	renderPath(cryph::Matrix4x4());

	// 5. Reestablish previous shader program
	glUseProgram(pgm);
	// ...
}

void Path::renderPath(cryph::Matrix4x4 M)
{
	glUniform1f(shaderIF->ppuLoc("snowScale"), 1);
	cryph::Matrix4x4 m_local;
	for(int i=0;i<n_prisms;i++){
		m_local = M * m_transform[i];
		prisms[i]->renderPrism(m_local);
		
	}
}
