// main.c++: Starter for EECS 672 Projects 2-4

#include "ExtendedController.h"
#include "Inline.h"
#include "SceneElements/Prism.h"
#include "SceneElements/Ground.h"
#include "SceneElements/SwingSet.h"
#include "SceneElements/LampPost.h"
#include "SceneElements/Bench.h"
#include "SceneElements/Path.h"
#include "SceneElements/ParticleSystem.h"
#include "SceneElements/Snowman.h"
#include "PhongMaterial.h"

void createScene(ExtendedController& c, ShaderIF* sIF, ShaderIF* particlesIF, ShaderIF* computesIF, ShaderIF* groundsIF)
{
	c.addModel(new Ground(groundsIF));
	cryph::AffVector center = cryph::AffVector(0, 0, 0);
	float scale = 0.30;
	c.addModel(new Bench(sIF, center+cryph::AffVector(-2.5, 0, -1), 0, scale));
	c.addModel(new Bench(sIF, center+cryph::AffVector(4.2, 0, 1.932), 270, scale));
	float bigPath[3] = {10.0, 2, 1.5};
	float smallPath[3] = {3.8, 2, 1.5};
	c.addModel(new Path(sIF, center+cryph::AffVector(-5, 0, 0), bigPath, 0, 0.5));
	c.addModel(new Path(sIF, center+cryph::AffVector(3, 0, 1.432), smallPath, 270, 0.5));
	c.addModel(new LampPost(sIF, center+cryph::AffVector(4, -0.1, -1.0), 0.75));
	c.addModel(new LampPost(sIF, center+cryph::AffVector(-4.5, 0, 1.932), 0.75));
	c.addModel(new SwingSet(sIF, center+cryph::AffVector(-1, 0, -4.5), 0.75));
	c.addModel(new Snowman(sIF, center+cryph::AffVector(-3.5, 0, -2.5), 0.75));
	c.addModel(new ParticleSystem(particlesIF, computesIF, 10000));
}

void set3DViewingInformation(double overallBB[])
{
	// IF we are using Viewing Strategy #1, THEN
	//     Notify class ModelView that we initially want to see the entire scene:
	//   ModelView::setMCRegionOfInterest(overallBB);
	ModelView::setMCRegionOfInterest(overallBB);
	// ELSE (Viewing Strategy #2)
	//     Modify all three of deltaX, deltaY, and deltaZ of the incoming overallBB
	//     to have the dimensions of the desired field of view, then invoke:
	//   ModelView::setMCRegionOfInterest(modified_overallBB);
	//   Tell the ModelView class that dynamic rotations are to be done about the eye.

	// MC -> EC:

	// TODO: Compute/set eye, center, up
	cryph::AffPoint eye, center;
	cryph::AffVector up;
	float center_arr[3];
	double r, d = 0;
	for(int i=0;i<6;i+=2){
		center_arr[i/2] = (overallBB[i+1] + overallBB[i]) / 2;
		r = cryph::maximum((overallBB[i+1] - overallBB[i]) / 2, r);
	}
	eye = cryph::AffPoint(0, 0, 1);
	up = cryph::AffPoint(0, 1, 0);
	center = cryph::AffPoint(center_arr);
	d = 9 * r;
	eye = center + d * eye;
	ModelView::setEyeCenterUp(eye, center, up);

	// EC -> LDS:

	// Specify the initial projection type desired
	ModelView::setProjection(PERSPECTIVE);

	// TODO: Compute/set ecZmin, ecZmax (It is often useful to exaggerate
	//       these limits somewhat to prevent unwanted depth clipping.)
	double ecZmin, ecZmax;
	// 1.5 is slightly larger than 2/sqrt(2). 2/sqrt(2) * r is the max distance to a corner of square.
	ecZmin = -d - 1.5 * r;
	ecZmax = -d + 1.5 * r;
	ModelView::setECZminZmax(ecZmin, ecZmax);

	// TODO: Compute/set ecZpp
	double ecZpp;
	ecZpp = ecZmax;
	ModelView::setProjectionPlaneZ(ecZpp);
}

int main(int argc, char* argv[])
{
	ExtendedController c("Bench Scene", MVC_USE_DEPTH_BIT);
	c.reportVersions(std::cout);


	ShaderIF* particlesIF = new ShaderIF("shaders/basicParticle.vsh", "shaders/phong.fsh");
	ShaderIF::ShaderSpec vertex  = {"shaders/basic.vsh", GL_VERTEX_SHADER}; 
	ShaderIF::ShaderSpec particleVertex  = {"shaders/basicParticle.vsh", GL_VERTEX_SHADER}; 
	ShaderIF::ShaderSpec groundVertex  = {"shaders/basicGround.vsh", GL_VERTEX_SHADER}; 
	ShaderIF::ShaderSpec fragment  = {"shaders/phong.fsh", GL_FRAGMENT_SHADER};		
	ShaderIF::ShaderSpec compute = {"shaders/compute.comp", GL_COMPUTE_SHADER};
	ShaderIF::ShaderSpec tessCtlShader  = {"shaders/tessCont.tsh", GL_TESS_CONTROL_SHADER};
	ShaderIF::ShaderSpec tessEvalShader = {"shaders/tess.tsh", GL_TESS_EVALUATION_SHADER};
	ShaderIF::ShaderSpec snowGeo = {"shaders/snowGeo.gsh", GL_GEOMETRY_SHADER};
	ShaderIF::ShaderSpec* computeShaders = new ShaderIF::ShaderSpec[1];
	computeShaders[0] = compute;
	
	ShaderIF::ShaderSpec* groundShaders = new ShaderIF::ShaderSpec[4];
	groundShaders[0] = groundVertex;
	groundShaders[1] = fragment;
	groundShaders[2] = tessCtlShader;
	groundShaders[3] = tessEvalShader;


	ShaderIF* sIF = new ShaderIF("shaders/basic.vsh", "shaders/phong.fsh");;
	ShaderIF* computesIF = new ShaderIF(computeShaders, 1);
	ShaderIF* groundsIF = new ShaderIF(groundShaders, 4);

	createScene(c, sIF, particlesIF, computesIF, groundsIF);


	double xyz[6];
	c.getOverallMCBoundingBox(xyz);
	std::cout << "Bounding box: " << xyz[0] << " <= x <= " << xyz[1] << '\n';
	std::cout << "              " << xyz[2] << " <= y <= " << xyz[3] << '\n';
	std::cout << "              " << xyz[4] << " <= z <= " << xyz[5] << "\n\n";
	std::cout << "Controls:\nSpacebar - Pause\nW - Full Winter\nS - Full Summer\n\n";
	set3DViewingInformation(xyz);
	c.setRunWaitsForAnEvent(false);
	c.run();

	c.removeAllModels(true);
	delete sIF;

	return 0;
}
