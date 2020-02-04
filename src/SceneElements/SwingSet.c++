// SwingSet.c++

#include "SwingSet.h"

SwingSet::SwingSet(ShaderIF* sIF, cryph::AffVector pos, float pScale) : SceneElement(sIF), position(pos), scale(pScale)
{
	initModelGeometry();
	calculateBoundingBox();
}

SwingSet::~SwingSet()
{
}

void SwingSet::calculateBoundingBox(){
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
void SwingSet::getMCBoundingBox(double* xyzLimits) const
{
	xyzLimits[0] = xyz[0]; // xmin  Give real values!
	xyzLimits[1] = xyz[1]; // xmax         |
	xyzLimits[2] = xyz[2]; // ymin         |
	xyzLimits[3] = xyz[3]; // ymax         |
	xyzLimits[4] = xyz[4]; // zmin         |
	xyzLimits[5] = xyz[5]; // zmax        \_/
}


void SwingSet::initModelGeometry(){
	components = new ModelView*[n_components];
	m_transform = new cryph::Matrix4x4[n_components];
	cryph::AffVector translate[n_components] = {
		cryph::AffVector(0, 0, 0),			//leftBaseFront
		cryph::AffVector(0.0, 0.0, 1.0),
		cryph::AffVector(0, 4.0, 0),
		cryph::AffVector(5.3, 0.0, 0),		//rightBaseFront
		cryph::AffVector(5.3, 0.0, 1.0),
		cryph::AffVector(5.3, 4.0, 0),
		cryph::AffVector(0.3, 4.0, 0.5),	//crossbar
		cryph::AffVector(1.3, 1.0, 0.375),	//swing1
		cryph::AffVector(3.6, 1.0, 0.375),
		cryph::AffVector(1.425, 1.15, 0.625),	//leftChain1
		cryph::AffVector(2.175, 1.15, 0.625),
		cryph::AffVector(3.725, 1.15, 0.625),
		cryph::AffVector(4.475, 1.15, 0.625),
	};
	float prismDimensions[9][3] = {
		{0.3, 4.1, 0.3},
		{0.3, 4.1, 0.3},
		{0.3, 0.3, 1.3},
		{0.3, 4.1, 0.3},
		{0.3, 4.1, 0.3},
		{0.3, 0.3, 1.3},
		{5.0, 0.3, 0.3},
		{1.0, 0.15, 0.5},
		{1.0, 0.15, 0.5},
	};
	leftBaseFront = new Prism(shaderIF, metalMaroon, prismDimensions[0], true, 0.05);
	leftBaseBack = new Prism(shaderIF, metalMaroon, prismDimensions[1], true, 0.05);
	leftBaseTop = new Prism(shaderIF, metalMaroon, prismDimensions[2], true, 0.05);
	rightBaseFront = new Prism(shaderIF, metalMaroon, prismDimensions[3], true, 0.05);
	rightBaseBack = new Prism(shaderIF, metalMaroon, prismDimensions[4], true, 0.05);
	rightBaseTop = new Prism(shaderIF, metalMaroon, prismDimensions[5], true, 0.05);
	crossbar = new Prism(shaderIF, metalMaroon, prismDimensions[6], true, 0.05);
	swing1 = new Prism(shaderIF, benchBrown, prismDimensions[7], true, 0.05);
	swing2 = new Prism(shaderIF, benchBrown, prismDimensions[8], true, 0.05);
	leftChain1 = new Cylinder(shaderIF, mushyMetal, 0.05, 2.85, "chain_cropped.png");
	leftChain2 = new Cylinder(shaderIF, mushyMetal, 0.05, 2.85, "chain_cropped.png");
	rightChain1 = new Cylinder(shaderIF, mushyMetal, 0.05, 2.85, "chain_cropped.png");
	rightChain2 = new Cylinder(shaderIF, mushyMetal, 0.05, 2.85, "chain_cropped.png");
	components[0] = leftBaseFront;
	components[1] = leftBaseBack;
	components[2] = leftBaseTop;
	components[3] = rightBaseFront;
	components[4] = rightBaseBack;
	components[5] = rightBaseTop;
	components[6] = crossbar;
	components[7] = swing1;
	components[8] = swing2;
	components[9] = leftChain1;
	components[10] = leftChain2;
	components[11] = rightChain1;
	components[12] = rightChain2;
	for(int i=0;i<n_components;i++){
		m_transform[i] = cryph::Matrix4x4(double(scale) * cryph::Matrix3x3(), translate[i] * double(scale) + position);
	}
}

void SwingSet::render()
{
	GLint pgm;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pgm);

	// draw the triangles using our vertex and fragment shaders
	glUseProgram(shaderIF->getShaderPgmID());

	renderSwingSet(cryph::Matrix4x4());

	// 5. Reestablish previous shader program
	glUseProgram(pgm);
	// ...
}

void SwingSet::renderSwingSet(cryph::Matrix4x4 M)
{
	glUniform1f(shaderIF->ppuLoc("snowScale"), scale);
	cryph::Matrix4x4 m_local;
	m_local = M * m_transform[0];
	leftBaseFront->renderPrism(m_local);
	m_local = M * m_transform[1];
	leftBaseBack->renderPrism(m_local);
	m_local = M * m_transform[2];
	leftBaseTop->renderPrism(m_local);
	m_local = M * m_transform[3];
	rightBaseFront->renderPrism(m_local);
	m_local = M * m_transform[4];
	rightBaseBack->renderPrism(m_local);
	m_local = M * m_transform[5];
	rightBaseTop->renderPrism(m_local);
	m_local = M * m_transform[6];
	crossbar->renderPrism(m_local);
	m_local = M * m_transform[7];
	swing1->renderPrism(m_local);
	m_local = M * m_transform[8];
	swing2->renderPrism(m_local);
	m_local = M * m_transform[9];
	leftChain1->renderCylinder(m_local);
	m_local = M * m_transform[10];
	rightChain1->renderCylinder(m_local);
	m_local = M * m_transform[11];
	leftChain2->renderCylinder(m_local);
	m_local = M * m_transform[12];
	rightChain2->renderCylinder(m_local);
	// 3. Set GLSL's "ka" and "kd" uniforms using this object's "ka" and "kd"
	//    instance variables
	// ...

	// 4. Establish any other attributes and make one or more calls to
	//    glDrawArrays and/or glDrawElements
	// ...
}
