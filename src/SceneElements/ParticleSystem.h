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
} Particle;


class ParticleSystem : public SceneElement
{
	

public:
	// As before: you will likely want to add parameters to the constructor
	ParticleSystem(ShaderIF* sIF, ShaderIF* computesIF, int n_par);
	virtual ~ParticleSystem();

	// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
	void getMCBoundingBox(double* xyzLimitsF) const;
	bool handleCommand(unsigned char anASCIIChar, double ldsX, double ldsY);
	void render();
	void renderParticleSystem(cryph::Matrix4x4 M);
private:
	int n_particles;
	Particle* particles;
	GLuint vao[1];
	GLuint vbo[1];
	ShaderIF* particleShaderIF;
	void particlePass();
	void initParticles();
	PhongMaterial solidSnow = PhongMaterial(1.0, 1.0, 1.0, 0.2, 0.9, 1.0, 10, 1);
	float last_time = -1;
	bool paused = false;
	float total_time = 0;
};

#endif
