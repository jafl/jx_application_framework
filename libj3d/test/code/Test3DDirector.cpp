/******************************************************************************
 Test3DDirector.cpp

	BASE CLASS = JXWindowDirector

	Written by John Lindal.

 ******************************************************************************/

#include <JXStdInc.h>
#include "Test3DDirector.h"
#include "Test3DWidget.h"
#include <J3DUniverse.h>
#include <J3DAxes.h>
#include <J3DTetrahedron.h>
#include <J3DCone.h>
#include <J3DTriangle.h>
#include <JXWindow.h>
#include <JXColormap.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

Test3DDirector::Test3DDirector
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

Test3DDirector::~Test3DDirector()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
Test3DDirector::BuildWindow()
{
	JXWindow* window = new JXWindow(this, 400,330, "Test 3D Director");
	assert( window != NULL );

	// create a universe (where else do you get to do that, eh?)

	itsUniverse = new J3DUniverse(*(GetColormap()));
	assert( itsUniverse != NULL );

	// create 2 cameras to test that they can coexist in one window

	its3DWidget1 =
		new Test3DWidget(itsUniverse, window,
						 JXWidget::kHElastic, JXWidget::kVElastic,
						 30,30, 200,200);
	assert( its3DWidget1 != NULL );

	its3DWidget2 =
		new Test3DWidget(itsUniverse, window,
						 JXWidget::kFixedRight, JXWidget::kFixedTop,
						 250,50, 100,200);
	assert( its3DWidget2 != NULL );

	// put something in the universe

	JXColormap* cmap = GetColormap();

	J3DAxes* axes = new J3DAxes(*cmap, itsUniverse, 2.0);
	assert( axes != NULL );

	J3DTetrahedron* tetr = new J3DTetrahedron(itsUniverse, 0,0,0);
	assert( tetr != NULL );
	tetr->SetVertexColor(J3DTetrahedron::kTopVertex,       cmap->GetWhiteColor());
	tetr->SetVertexColor(J3DTetrahedron::kFrontVertex,     cmap->GetRedColor());
	tetr->SetVertexColor(J3DTetrahedron::kBackLeftVertex,  cmap->GetGreenColor());
	tetr->SetVertexColor(J3DTetrahedron::kBackRightVertex, cmap->GetBlueColor());
	tetr->SetRotation(JVector(3, 1.0,1.0,1.0), 0.0);

	J3DCylinder* cyl = new J3DCylinder(itsUniverse, 0,2,0, 1, 0.5, 1);
	assert( cyl != NULL );
	cyl->SetColor(cmap->GetDarkRedColor());

	J3DCone* cone = new J3DCone(itsUniverse, 1.0,-1.0,-0.5, 0.5, 0.25);
	assert( cone != NULL );
	cone->SetRotation(JVector(3, 1.0,1.0,1.0), 0.0);
	cone->SetScale(JVector(3, 2.0,2.0,2.0));

	J3DTriangle* tri = new J3DTriangle(itsUniverse,
									   JVector(3,-0.2,-0.2,-0.2),
									   JVector(3,-0.5, 0.0, 0.0),
									   JVector(3,-0.5,-0.5, 0.1));
	assert( tri != NULL );
	tri->SetColor(cmap->GetOrangeColor());
	tri->SetVertexColor(2, cmap->GetRedColor());
}
