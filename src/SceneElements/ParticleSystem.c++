// ParticleSystem.c++ - a basic combined Model and View for OpenGL

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <math.h>

#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(ShaderIF* sIF, ShaderIF* particleUpdatesIF, ShaderIF* physicsUpdatesIF, int n_par) : SceneElement(sIF), particleUpdatesIF(particleUpdatesIF), physicsUpdatesIF(physicsUpdatesIF), n_particles(n_par)
{
	initParticles();
}

ParticleSystem::~ParticleSystem()
{
    glDeleteBuffers(3, vbo);
    glDeleteVertexArrays(1, vao);
	delete[] particles;
	delete[] positions;
	delete[] hash_table;
	
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
	hash_size = 151;
	particles = new Particle[n_particles];
	positions = new vec3[n_particles];
	hash_table = new uint32_t[hash_size];
	uint32_t hash_index;
	for(int i=0;i<hash_size;i++)
		hash_table[i] = -1;
	for(int i=0;i<n_particles;i++){
		particles[i].position[0] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 10 - 5;
		particles[i].position[1] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 10 - 5;
		particles[i].position[2] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 10 - 5;

		particles[i].nextParticle = -1;
		particles[i].frameUpdated = 0;
		

		positions[i][0] = particles[i].position[0];
		positions[i][1] = particles[i].position[1];
		positions[i][2] = particles[i].position[2];

        particles[i].origin[0] = particles[i].position[0];
        particles[i].origin[1] = particles[i].position[1];
        particles[i].origin[2] = particles[i].position[2];

		particles[i].velocity[0] = 0; //(static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.2 - 0.1;
		particles[i].velocity[1] = 0; //-((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.5 + 0.5);
		particles[i].velocity[2] = 0; //(static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.2 - 0.1;
	}
	for(int i=0;i<n_particles;i++){
		hash_index = (uint32_t(floor(particles[i].position[0] / d) * p1) ^ uint32_t(floor(particles[i].position[1] / d) * p2) ^ uint32_t(floor(particles[i].position[2] / d) * p3)) % hash_size;
		// std::cout << hash_index << std::endl;
		if(hash_table[hash_index] == -1){
			// std::cout << hash_table[hash_index]  << std::endl;
			hash_table[hash_index] = i;
		}
		else{
			int index = hash_table[hash_index];
			int cur = index;
			while(index != -1){
				cur = index;
				std::cout << cur << std::endl;
				index = particles[cur].nextParticle;
			}
			particles[cur].nextParticle = i;
		}

	}

	glGenVertexArrays(1, vao);
	glGenBuffers(3, vbo);

	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

	int numBytesInBuffer = n_particles * sizeof(Particle);
	glBufferData(GL_ARRAY_BUFFER, numBytesInBuffer, particles, GL_STATIC_DRAW);
	glVertexAttribPointer(shaderIF->pvaLoc("mcPosition"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle), reinterpret_cast<void*>(0 + offsetof(Particle, position)));
	glEnableVertexAttribArray(shaderIF->pvaLoc("mcPosition"));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	numBytesInBuffer = n_particles * sizeof(vec3);
	glBufferData(GL_ARRAY_BUFFER, numBytesInBuffer, positions, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	numBytesInBuffer = hash_size * sizeof(uint32_t);
	glBufferData(GL_ARRAY_BUFFER, numBytesInBuffer, hash_table, GL_STATIC_DRAW);
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
	glUseProgram(particleUpdatesIF->getShaderPgmID());

	glUniform1i(particleUpdatesIF->ppuLoc("current_frame"), current_frame);
	glUniform1f(particleUpdatesIF->ppuLoc("hash_size"), hash_size);

	if(last_time == -1)
		last_time = glfwGetTime();
    float delta_time = glfwGetTime() - last_time;
    if(!paused)
        total_time += delta_time;
	glUniform1f(particleUpdatesIF->ppuLoc("deltaTime"), glfwGetTime() - last_time);
	last_time = glfwGetTime();

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, vbo[0]);

	int warp_count = ceil((float)n_particles / (float)256);

	glDispatchCompute(warp_count, 1, 1);
	glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
}

void ParticleSystem::physicsPass(){
	glUseProgram(physicsUpdatesIF->getShaderPgmID());

	
	glUniform1i(physicsUpdatesIF->ppuLoc("current_frame"), current_frame);
	glUniform1f(physicsUpdatesIF->ppuLoc("hash_size"), hash_size);
	

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, vbo[0]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, vbo[1]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, vbo[1]);

	int warp_count = ceil((float)n_particles / (float)256);

	// glDispatchCompute(warp_count, 1, 1);
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
	current_frame = (current_frame + 1) % 256;
	// physicsPass();
	particlePass();
	// restore the previous program
	glUseProgram(pgm);
}

