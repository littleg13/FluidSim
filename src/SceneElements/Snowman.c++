// Snowman.c++

#include "Snowman.h"

Snowman::Snowman(ShaderIF* sIF, cryph::AffVector pos, float pScale) : SceneElement(sIF), position(pos), scale(pScale)
{
	initModelGeometry();
	calculateBoundingBox();
}

Snowman::~Snowman()
{
	for(int i=0;i<n_components;i++){
		delete components[i];
		delete renderers[i];
	}
}

void Snowman::calculateBoundingBox(){
	xyz[0] = 1.0; xyz[1] = 0.0;
	for(int i=0;i<n_components;i++){
		// accumulate bounding box
		if (xyz[0] > xyz[1]) // not yet initialized
			components[i]->getMCBoundingBox(xyz);
		else
		{
			double thisxyz[6];
			components[i]->getMCBoundingBox(thisxyz);
			for (int j=0 ; j<3 ; j++)
			{
				if (thisxyz[2*j] < xyz[2*j])
					xyz[2*j] = thisxyz[2*j];
				if (thisxyz[2*j+1] > xyz[2*j+1])
					xyz[2*j+1] = thisxyz[2*j+1];
			}
		}
	}
}

// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
void Snowman::getMCBoundingBox(double* xyzLimits) const
{
	xyzLimits[0] = xyz[0]; // xmin  Give real values!
	xyzLimits[1] = xyz[1]; // xmax         |
	xyzLimits[2] = xyz[2]; // ymin         |
	xyzLimits[3] = xyz[3]; // ymax         |
	xyzLimits[4] = xyz[4]; // zmin         |
	xyzLimits[5] = xyz[5]; // zmax        \_/
}


void Snowman::initModelGeometry(){
	float bigRadius = 0.75;
	float medRadius = bigRadius * 2 / 3;
	float smallRadius = medRadius * 2 / 3;
	cryph::AffPoint origin = cryph::AffPoint();
	cryph::AffPoint botCenter = origin + cryph::AffVector::yu * bigRadius;
	cryph::AffPoint midCenter = botCenter + cryph::AffVector::yu * (bigRadius * 0.9 + medRadius);
	cryph::AffPoint topCenter = midCenter + cryph::AffVector::yu * (medRadius * 0.9 + smallRadius);
	components[0] = BasicShape::makeSphere(botCenter, bigRadius, 100, 100);
	components[1] = BasicShape::makeSphere(midCenter, medRadius, 100, 100);
	components[2] = BasicShape::makeSphere(topCenter, smallRadius, 100, 100);
	float armRadius = 0.1;
	float armLength = 0.5;
	cryph::AffVector rightArmDir = cryph::AffVector(1, -0.3, 0);
	cryph::AffVector leftArmDir = cryph::AffVector(-1, -0.3, 0);
	components[3] = BasicShape::makeBoundedCylinder(midCenter + (medRadius * 0.9) * cryph::AffVector::xu, midCenter + (armLength + medRadius * 0.9) * rightArmDir, armRadius, 20, 2);
	components[4] = BasicShape::makeBoundedCylinder(midCenter - (medRadius * 0.9) * cryph::AffVector::xu, midCenter + (armLength + medRadius * 0.9) * leftArmDir, armRadius, 20, 2);
	float noseLength = 0.2;
	float noseRadius = 0.1;
	cryph::AffPoint rightEye = topCenter + cryph::AffVector(smallRadius / 3.0, smallRadius / 3.0, smallRadius * 0.9);
	cryph::AffPoint leftEye = topCenter + cryph::AffVector(-smallRadius / 3.0, smallRadius / 3.0, smallRadius * 0.9);
	components[5] = BasicShape::makeBoundedCone(topCenter + (smallRadius * 0.9) * cryph::AffVector::zu, topCenter + (smallRadius * 0.9 + noseLength) * cryph::AffVector::zu, noseRadius, 0.01, 20, 2);
	components[6] = BasicShape::makeSphere(rightEye, noseRadius * 0.5, 20, 20);
	components[7] = BasicShape::makeSphere(leftEye, noseRadius * 0.5, 20, 20);
	materials[0] = solidSnow;
	materials[1] = solidSnow;
	materials[2] = solidSnow;
	materials[3] = benchBrown;
	materials[4] = benchBrown;
	materials[5] = carrotCrayon;
	materials[6] = blackBall;
	materials[7] = blackBall;

	for(int i=0;i<n_components;i++){
		renderers[i] = new BasicShapeRenderer(shaderIF, components[i]);
	}
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, readTextureImage("Noise.png"));
}

void Snowman::render()
{
	GLint pgm;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pgm);

	// draw the triangles using our vertex and fragment shaders
	glUseProgram(shaderIF->getShaderPgmID());

	float season = establishSeason();
	glUniform2f(shaderIF->ppuLoc("snowMapDim"), 10, 10);
	float currentScale = scale * 0.9 * season;
	if(currentScale < 0.0)
		currentScale = 0.0;
	glUniform1f(shaderIF->ppuLoc("snowScale"), 10 * currentScale);
	renderSnowman(cryph::Matrix4x4(currentScale * cryph::Matrix3x3(), position));

	// 5. Reestablish previous shader program
	glUseProgram(pgm);
	// ...
}

void Snowman::prepareForFace(void* caller, int faceIndex)
{
}

void Snowman::renderSnowman(cryph::Matrix4x4 M)
{
	establishView(M);
	establishLightingEnvironment();
	for(int i=0;i<n_components;i++)
		if (renderers[i] != nullptr){
			establishMaterial(materials[i]);
			renderers[i]->renderShape(prepareForFace, this);
		}
}
