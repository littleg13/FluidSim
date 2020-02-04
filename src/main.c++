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

void createScene(ExtendedController& c, ShaderIF* particlesIF, ShaderIF* computesIF)
{
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
	ShaderIF::ShaderSpec compute = {"shaders/compute.comp", GL_COMPUTE_SHADER};
	
	ShaderIF::ShaderSpec* computeShaders = new ShaderIF::ShaderSpec[1];
	computeShaders[0] = compute;
	
	ShaderIF* computesIF = new ShaderIF(computeShaders, 1);

	createScene(c, particlesIF, computesIF);


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

	return 0;
}
