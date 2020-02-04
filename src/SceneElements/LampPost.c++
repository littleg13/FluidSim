// LampPost.c++

#include "LampPost.h"

LampPost::LampPost(ShaderIF* sIF, cryph::AffVector pos, float pScale) : SceneElement(sIF), position(pos), scale(pScale)
{
	initModelGeometry();
	calculateBoundingBox();
}

LampPost::~LampPost()
{
}

void LampPost::calculateBoundingBox(){
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
void LampPost::getMCBoundingBox(double* xyzLimits) const
{
	xyzLimits[0] = xyz[0]; // xmin  Give real values!
	xyzLimits[1] = xyz[1]; // xmax         |
	xyzLimits[2] = xyz[2]; // ymin         |
	xyzLimits[3] = xyz[3]; // ymax         |
	xyzLimits[4] = xyz[4]; // zmin         |
	xyzLimits[5] = xyz[5]; // zmax        \_/
}


void LampPost::initModelGeometry(){
	components = new ModelView*[n_components];
	m_transform = new cryph::Matrix4x4[n_components];
	cryph::AffVector translate[n_components] = {
		cryph::AffVector(0, 0, 0),
		cryph::AffVector(0.25, 0.25, 0.25),
		cryph::AffVector(0, 3.25, 0),
		cryph::AffVector(-0.05, 4.0, -0.05),
	};
	float prismDimensions[3][3] = {
		{0.5, 0.25, 0.5},
		{0.5, 0.75, 0.5},
		{0.6, 0.2, 0.6}
	};
	base = new Prism(shaderIF, loomingGreyLampPost, prismDimensions[0], true, 0.05);
	pole = new Cylinder(shaderIF, loomingGreyLampPost, 0.1, 3);
	lamp = new Prism(shaderIF, washedWhite, prismDimensions[1], true, 0.01);
	lid = new Prism(shaderIF, loomingGreyLampPost, prismDimensions[2], true, 0.01);
	components[0] = base;
	components[1] = pole;
	components[2] = lamp;
	components[3] = lid;
	for(int i=0;i<n_components;i++){
		m_transform[i] = cryph::Matrix4x4(double(scale) * cryph::Matrix3x3(), translate[i] * double(scale) + position);
	}
}

void LampPost::render()
{
	GLint pgm;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pgm);

	// draw the triangles using our vertex and fragment shaders
	glUseProgram(shaderIF->getShaderPgmID());

	renderLampPost(cryph::Matrix4x4());

	// 5. Reestablish previous shader program
	glUseProgram(pgm);
	// ...
}

void LampPost::renderLampPost(cryph::Matrix4x4 M)
{
	glUniform1f(shaderIF->ppuLoc("snowScale"), scale);
	cryph::Matrix4x4 m_local;
	m_local = M * m_transform[0];
	base->renderPrism(m_local);
	m_local = M * m_transform[1];
	pole->renderCylinder(m_local);
	m_local = M * m_transform[2];
	lamp->renderPrism(m_local);
	m_local = M * m_transform[3];
	lid->renderPrism(m_local);
	// 3. Set GLSL's "ka" and "kd" uniforms using this object's "ka" and "kd"
	//    instance variables
	// ...

	// 4. Establish any other attributes and make one or more calls to
	//    glDrawArrays and/or glDrawElements
	// ...
}
