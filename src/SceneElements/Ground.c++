// Ground.c++

#include "Ground.h"

Ground::Ground(ShaderIF* sIF) : SceneElement(sIF)
{
	initModelGeometry();
	calculateBoundingBox();
}

Ground::~Ground()
{
	delete noiseImage;
	delete[] vertices;
	delete[] normals;
	glDeleteBuffers(3, vbo);
    glDeleteVertexArrays(1, vao);
}

void Ground::calculateBoundingBox(){
	xmin = zmin = -size;
	xmax = zmax = size;
	ymin = ymax = 0;
}

// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
void Ground::getMCBoundingBox(double* xyzLimits) const
{
	xyzLimits[0] = xmin; // xmin  Give real values!
	xyzLimits[1] = xmax; // xmax         |
	xyzLimits[2] = ymin; // ymin         |
	xyzLimits[3] = ymax; // ymax         |
	xyzLimits[4] = zmin; // zmin         |
	xyzLimits[5] = zmax; // zmax        \_/
}


void Ground::initModelGeometry(){
	precision -= 1; //We are only concerned in between rows
	float increment = size / ((float)precision);
	int count = 0;
	float height = 5.4;
	int indexCount = 0;
	n_vertices = precision * precision * 4;
	vertices = new vec3[n_vertices];
	normals = new vec3[n_vertices];
	texCoords = new vec2[n_vertices];
	float width = size / 2.0;
	for(int i=0;i<precision;i++){
		for(int j=0;j<precision;j++){
			vertices[count][0] = i * increment - width;
			vertices[count][2] = j * increment - width;
			vertices[count][1] = 0;
			cryph::AffVector(0, 1, 0).vComponents(normals[count]);
			texCoords[count][0] = (texTiling / (float)precision) * i;
			texCoords[count][1] = (texTiling / (float)precision) * j;
			count++;
			vertices[count][0] = (i + 1) * increment - width;
			vertices[count][2] = j * increment - width;
			vertices[count][1] = 0;
			cryph::AffVector(0, 1, 0).vComponents(normals[count]);
			texCoords[count][0] = (texTiling / (float)precision) * (i + 1);
			texCoords[count][1] = (texTiling / (float)precision) * j;
			count++;
			vertices[count][0] = i * increment - width;
			vertices[count][2] = (j + 1) * increment - width;
			vertices[count][1] = 0;
			cryph::AffVector(0, 1, 0).vComponents(normals[count]);
			texCoords[count][0] = (texTiling / (float)precision) * i;
			texCoords[count][1] = (texTiling / (float)precision) * (j + 1);
			count++;
			vertices[count][0] = (i + 1) * increment - width;
			vertices[count][2] = (j + 1) * increment - width;
			vertices[count][1] = 0;
			cryph::AffVector(0, 1, 0).vComponents(normals[count]);
			texCoords[count][0] = (texTiling / (float)precision) * (i + 1);
			texCoords[count][1] = (texTiling / (float)precision) * (j + 1);
			count++;
		}
	}
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(3, vbo);

	int numBytes = sizeof(float) * 3 * n_vertices;
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
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

	glActiveTexture(GL_TEXTURE0);
	texID[0] = readTextureImage("Grass.jpg");
	glActiveTexture(GL_TEXTURE1);
	readTextureImage("Noise.png");
	
}


void Ground::render()
{
	GLint pgm;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pgm);

	// draw the triangles using our vertex and fragment shaders
	glUseProgram(shaderIF->getShaderPgmID());

	renderGround();
	establishView(cryph::Matrix4x4());

	// 5. Reestablish previous shader program
	glUseProgram(pgm);
	// ...
}

void Ground::renderGround()
{
	glBindVertexArray(vao[0]);
	glUniform1f(shaderIF->ppuLoc("snowScale"), 1);
	establishSeason();
	establishTexture(texID[0], false, GL_REPEAT);
	establishMaterial(grassGreen);
	establishLightingEnvironment();

	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	glUniform2i(shaderIF->ppuLoc("vpWidthHeight"), vp[2], vp[3]);
	glUniform1f(shaderIF->ppuLoc("maxPixelLength"), 15.0);

	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawArrays(GL_PATCHES, 0, precision * precision * 4);

}
