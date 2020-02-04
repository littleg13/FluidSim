// ParticleSystem.c++ - a basic combined Model and View for OpenGL

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <math.h>

#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(ShaderIF* sIF, ShaderIF* computesIF, int n_par) : SceneElement(sIF), particleShaderIF(computesIF), n_particles(n_par)
{
	initParticles();
}

ParticleSystem::~ParticleSystem()
{
    glDeleteBuffers(1, vbo);
    glDeleteVertexArrays(1, vao);
	delete[] particles;
	
}

void ParticleSystem::getMCBoundingBox(double* xyzLimits) const
{
	xyzLimits[0] = -5; // xmin  Give real values!
	xyzLimits[1] = 5; // xmax         |
	xyzLimits[2] = -5; // ymin         |
	xyzLimits[3] = 5; // ymax         |
	xyzLimits[4] = -5; // zmin         |
	xyzLimits[5] = 5; // zmax        \_/
}

bool ParticleSystem::handleCommand(unsigned char anASCIIChar, double ldsX, double ldsY)
{
	return true;
}

void ParticleSystem::initParticles(){
	particles = new Particle[n_particles];
	for(int i=0;i<n_particles;i++){
		particles[i].position[0] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 10 - 5;
		particles[i].position[1] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 10 - 5;
		particles[i].position[2] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 10 - 5;

        particles[i].origin[0] = particles[i].position[0];
        particles[i].origin[1] = particles[i].position[1];
        particles[i].origin[2] = particles[i].position[2];

		particles[i].velocity[0] = 0; //(static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.2 - 0.1;
		particles[i].velocity[1] = 0; //-((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.5 + 0.5);
		particles[i].velocity[2] = 0; //(static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.2 - 0.1;
	}

	glGenVertexArrays(1, vao);
	glGenBuffers(1, vbo);

	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

	int numBytesInBuffer = n_particles * sizeof(Particle);
	glBufferData(GL_ARRAY_BUFFER, numBytesInBuffer, particles, GL_STATIC_DRAW);
	glVertexAttribPointer(shaderIF->pvaLoc("mcPosition"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle), reinterpret_cast<void*>(0 + offsetof(Particle, position)));
	glEnableVertexAttribArray(shaderIF->pvaLoc("mcPosition"));
	// glVertexAttribPointer(shaderIF->pvaLoc("mcVelocity"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle), reinterpret_cast<void*>(0 + offsetof(Particle, velocity)));
	// glEnableVertexAttribArray(shaderIF->pvaLoc("mcVelocity"));
	glPointSize(1.0);


	glUseProgram(shaderIF->getShaderPgmID());
	
}

void ParticleSystem::renderParticleSystem(cryph::Matrix4x4 M){
	// draw the triangles using our vertex and fragment shaders
	glUseProgram(shaderIF->getShaderPgmID());

	glBindVertexArray(vao[0]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

    float mat[16];
	glUniformMatrix4fv(shaderIF->ppuLoc("mc_ec"), 1, false, M.extractColMajor(mat));
	establishMaterial(solidSnow);
	establishLightingEnvironment();

	glDrawArrays(GL_POINTS, 0, n_particles);
}

void ParticleSystem::particlePass(){
	glUseProgram(particleShaderIF->getShaderPgmID());

	if(last_time == -1)
		last_time = glfwGetTime();
    float delta_time = glfwGetTime() - last_time;
    if(!paused)
        total_time += delta_time;
	glUniform1f(particleShaderIF->ppuLoc("deltaTime"), glfwGetTime() - last_time);
	last_time = glfwGetTime();

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, vbo[0]);

	int warp_count = ceil((float)n_particles / (float)256);

	glDispatchCompute(warp_count, 1, 1);
	glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
}


void ParticleSystem::render()
{
	// save the current GLSL program in use
	GLint pgm;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pgm);
    cryph::Matrix4x4 mc_ec, ec_lds;
	getMatrices(mc_ec, ec_lds);
	float mat[16];
	glUniformMatrix4fv(shaderIF->ppuLoc("ec_lds"), 1, false, ec_lds.extractColMajor(mat));
    
	renderParticleSystem(mc_ec);
	particlePass();
	// restore the previous program
	glUseProgram(pgm);
}

