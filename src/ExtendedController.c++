// ExtendedController.c++

#include "ExtendedController.h"
#include "ModelView.h"

ExtendedController::ExtendedController(const std::string& name, int rcFlags):
	GLFWController(name, rcFlags)
{
}

void ExtendedController::handleMouseMotion(int x, int y)
{
	int dx = x - screenBaseX;
	int dy = y - screenBaseY;
	screenBaseX = x;
	screenBaseY = y;
	if(mouseMotionIsTranslate){
		double ldsX, ldsY;
		pixelVectorToLDSVector(dx, dy, ldsX, ldsY);
		ModelView::addToGlobalPan(ldsX, ldsY, 0);
	}
	else if(mouseMotionIsRotate){
		double rotFactor = 1;
		double rotX = rotFactor * dy;
		double rotY = rotFactor * dx;
		ModelView::addToGlobalRotationDegrees(rotX, rotY, 0);
	}
	redraw();
}

void ExtendedController::handleDisplay()
{
    prepareWindow(); // an inherited GLFWController method needed for some platforms
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw opaque objects
    glDisable(GL_BLEND);
    drawingOpaqueObjects = true; // record in instance variable so ModelView instances can query
    renderAllModels();

    // draw translucent objects
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    drawingOpaqueObjects = false; // record in instance variable so ModelView instances can query
    renderAllModels();

    swapBuffers();
}

// The following must a public method in class ExtendedController:
bool ExtendedController::drawingOpaque() const // CALLED FROM SceneElement or descendant classes
{
    return drawingOpaqueObjects;
}