// ParticleSystem.h

#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "SceneElement.h"
#include "GLFWController.h"

typedef float vec3[3];
typedef float vec4[4];
typedef struct Particle
{
	vec4 position;
	vec4 velocity;
	vec4 origin;
	int nextParticle;
	int frameUpdated;
} Particle;


class ParticleSystem : public SceneElement
{
	

public:
	// As before: you will likely want to add parameters to the constructor
	ParticleSystem(ShaderIF* sIF, ShaderIF* particleUpdatesIF, ShaderIF* physicsUpdatesIF, int n_par);
	virtual ~ParticleSystem();

	// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
	void getMCBoundingBox(double* xyzLimitsF) const;
	bool handleCommand(unsigned char anASCIIChar, double ldsX, double ldsY);
	void render();
	void renderParticleSystem(cryph::Matrix4x4 M);
private:
	int n_particles, hash_size;
	int current_frame = 0;
	//Random large prime numbers
	uint32_t p1 = 73856093; 
	uint32_t p2 = 19349663;
	uint32_t p3 = 83492791;
	//Cell spacing
	float d = 1;
	Particle* particles;
	vec3* positions;
	uint32_t* hash_table;
	GLuint vao[1];
	GLuint vbo[3];
	ShaderIF* particleUpdatesIF;
	ShaderIF* physicsUpdatesIF;
	void particlePass();
	void physicsPass();
	void initParticles();
	PhongMaterial solidSnow = PhongMaterial(1.0, 1.0, 1.0, 0.2, 0.9, 1.0, 10, 1);
	float last_time = -1;
	bool paused = false;
	float total_time = 0;
};

#endif
