// Prism.c++

#include "Prism.h"

Prism::Prism(ShaderIF* sIF, PhongMaterial& matlIn, vec3 dim, bool closed, float cornerRadius) : SceneElement(sIF), matl(matlIn), radius(cornerRadius)
{
	n_faces = closed ? 6 : 4;
	for(int i=0;i<3;i++){
		dimensions[i] = dim[i];
	}
	initModelGeometry();
	calculateBoundingBox();
}

Prism::~Prism()
{
	delete[] vertices;
	delete[] normals;
	glDeleteBuffers(1, ebo);
	glDeleteBuffers(2, vbo);
	glDeleteVertexArrays(1, vao);
}

void Prism::calculateBoundingBox(){
	xmin = ymin = zmin = 0;
	xmax = dimensions[0];
	ymax = dimensions[1];
	zmax = dimensions[2];
}

// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
void Prism::getMCBoundingBox(double* xyzLimits) const
{
	xyzLimits[0] = xmin; // xmin  Give real values!
	xyzLimits[1] = xmax; // xmax         |
	xyzLimits[2] = ymin; // ymin         |
	xyzLimits[3] = ymax; // ymax         |
	xyzLimits[4] = zmin; // zmin         |
	xyzLimits[5] = zmax; // zmax        \_/
}

void Prism::generateCorners(int anglePrecision){
	cryph::AffPoint center = cryph::AffPoint(dimensions) / 2;
	int newVertices = anglePrecision / 4 + 1;
	newVertices *= newVertices;
	cryph::AffPoint* quadrantPoints = new cryph::AffPoint[newVertices];
	int count = 0;
	for(int phi=0;phi<=90;phi+=360/anglePrecision){
		for(int theta=0;theta<=90;theta+=360/anglePrecision){
			cryph::AffPoint p = center - cryph::AffVector::xu * radius;
			quadrantPoints[count] = cryph::Matrix4x4::generalRotationDegrees(center, cryph::AffVector::xu, theta) * cryph::Matrix4x4::generalRotationDegrees(center, cryph::AffVector::zu, phi) * p;
			count++;
		}
	}
	int corners[8][3] = {
		{-1,-1,-1},
		{-1,1,-1},
		{-1,1,1},
		{-1,-1,1},
		{1,-1,-1},
		{1,1,-1},
		{1,1,1},
		{1,-1,1},
	};
	for(int i=0;i<8;i++){
		cryph::AffVector translation = cryph::AffVector(
			corners[i][0] * (dimensions[0] / 2 - radius),
			corners[i][1] * (dimensions[1] / 2 - radius),
			corners[i][2] * (dimensions[2] / 2 - radius)
		);
		cryph::Matrix4x4 transform = cryph::Matrix4x4::generalRotationDegrees(center, cryph::AffVector::xu, 90 * (i%4));
		if(i/4 == 1)
			transform *= cryph::Matrix4x4::mirror(center, cryph::AffVector::xu);
		for(int j=0;j<newVertices;j++){
			cryph::AffPoint p = transform * quadrantPoints[j];
			cryph::AffVector normal = p - center;
			p += translation;
			normal.normalize();
			p.aCoords(vertices[vboShift]);
			normal.vComponents(normals[vboShift]);
			vboShift++;
		}
	}
	int cornerOffset = 144;
	int quadPerRowCorner = anglePrecision / 4; //6
	int verticesPerRow = quadPerRowCorner + 1;
	for(int i=0;i<8;i++){
		int indexOffset = cornerOffset + i * newVertices;
		for(int j=0;j<quadPerRowCorner;j++){
			for(int k=0;k<quadPerRowCorner;k++){
				addQuad(indexOffset + k, indexOffset + verticesPerRow + k, indexOffset + k + 1, indexOffset + verticesPerRow + k + 1);
			}
			indexOffset += verticesPerRow;
		}
	}
	delete[] quadrantPoints;
}

void Prism::generateEdges(int anglePrecision){
	cryph::AffPoint center = cryph::AffPoint(dimensions) / 2;
	for(int i=0;i<3;i++){
		vec3 axisArr = {0, 0, 0};
		vec3 uArr = {0, 0, 0};
		axisArr[i] = 1;
		uArr[(i+1)%3] = 1;
		cryph::AffVector axis = cryph::AffVector(axisArr);
		cryph::AffVector U = cryph::AffVector(uArr);
		cryph::AffVector V = axis.cross(U);
		for(int theta=0;theta<360;theta+=360/anglePrecision){
			//Skip the vertices on the edge since we have those from the faces
			if(theta%90 == 0)
				continue;
			cryph::AffPoint p1 = center;
			p1 = p1 - axis * (dimensions[i] / 2 - radius) + U * radius;
			p1 = cryph::Matrix4x4::generalRotationDegrees(center, axis, theta) * p1;
			cryph::AffVector normal = p1 - (center - axis * (dimensions[i] / 2 - radius));
			normal.normalize();
			cryph::AffVector uComp = (U.dot(normal) * U);
			cryph::AffVector vComp = (V.dot(normal) * V);
			uComp.normalize();
			vComp.normalize();
			p1 += uComp * (dimensions[(i + 1)%3]/2 - radius) + vComp * (dimensions[(i + 2)%3]/2 - radius);
			cryph::AffPoint p2 = p1 + axis * (dimensions[i] - 2 * radius);
			p1.aCoords(vertices[vboShift]);
			normal.vComponents(normals[vboShift]);
			vboShift++;
			p2.aCoords(vertices[vboShift]);
			normal.vComponents(normals[vboShift]);
			vboShift++;
		}
	}
	int edgeBoundaries[12][4] = {
		{13,15,22,23},		//0
		{20,21,9,11},		//1
		{8,10,16,17},		//2
		{18,19,12,14},		//3
		{21,23,6,7},		//4
		{4,5,17,19},		//5
		{16,18,0,1},		//6
		{2,3,20,22},		//7
		{5,7,14,15},		//8
		{12,13,1,3},		//9
		{0,2,8,9},			//10
		{10,11,4,6}			//11
	};
	int edgeOffset = 24;
	int verticesPerEdge = (anglePrecision - 4)/2;
	int quadsPerEdge = verticesPerEdge/2 - 1;
	for(int i=0;i<12;i++){
		int indexOffset = edgeOffset + i * verticesPerEdge;
		//Connect to existing vertices
		addQuad(edgeBoundaries[i][0], edgeBoundaries[i][1], indexOffset, indexOffset + 1);
		for(int j=0;j<quadsPerEdge;j++){
			addQuad(indexOffset + j*2, indexOffset + j*2 + 1, indexOffset + j*2 + 2, indexOffset + j*2 + 3);
		}
		//Connect to existing vertices
		addQuad(indexOffset + verticesPerEdge - 2, indexOffset + verticesPerEdge - 1, edgeBoundaries[i][2], edgeBoundaries[i][3]);
	}
}

void Prism::initModelGeometry(){
	for(int i=0;i<24;i++){
		for(int j=0;j<3;j++){
			float offset = 0;
			int inDirection = 1;
			if(j!=i/8){
				offset = radius;
				inDirection = 0;
			}
			// I apologize for this
			// maxOrMin = 1 when coordinate should be at max of box, 0 when at min
			int maxOrMin = (i >> ((j+2-(i/8))%3)) & 1;
			vertices[vboShift][j] = center[j] + offset + (dimensions[j] - 2 * offset) * maxOrMin;
			normals[vboShift][j] = inDirection * (-1 + 2 * maxOrMin);
		}
		vboShift++;
	}
	for(int i=0;i<n_faces;i++){
		int offset = i*4;
		addQuad(offset, offset+1, offset+2, offset+3);
	}
	generateEdges(24);
	generateCorners(24);
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(2, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	int numBytes = sizeof(float) * 3 * n_vertices;
	glBufferData(GL_ARRAY_BUFFER, numBytes, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(shaderIF->pvaLoc("mcPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shaderIF->pvaLoc("mcPosition"));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, numBytes, normals, GL_STATIC_DRAW);
	glVertexAttribPointer(shaderIF->pvaLoc("mcNormal"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shaderIF->pvaLoc("mcNormal"));

	
	glGenBuffers(1, ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 3 * indexShift, indicesList, GL_STATIC_DRAW);
}


void Prism::render()
{
	GLint pgm;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pgm);

	// draw the triangles using our vertex and fragment shaders
	glUseProgram(shaderIF->getShaderPgmID());

	// 2. Establish "mc_ec" and "ec_lds" matrices
	// ...
	renderPrism(cryph::Matrix4x4());

	// 5. Reestablish previous shader program
	glUseProgram(pgm);
	// ...
}

void Prism::renderPrism(cryph::Matrix4x4 M)
{
	establishView(M);
	establishMaterial(matl);
	establishLightingEnvironment();

	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
	glDrawElements(GL_TRIANGLES, 3 * indexShift, GL_UNSIGNED_INT, nullptr);
}

void Prism::addQuad(int v00, int v01, int v10, int v11){
	indicesList[indexShift][0] = v00;
	indicesList[indexShift][1] = v01;
	indicesList[indexShift][2] = v10;
	indexShift++;
	indicesList[indexShift][0] = v10;
	indicesList[indexShift][1] = v11;
	indicesList[indexShift][2] = v01;
	indexShift++;
}
