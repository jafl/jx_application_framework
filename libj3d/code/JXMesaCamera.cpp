/******************************************************************************
 JXMesaCamera.cpp

	BASE CLASS = J3DCamera, J3DPainter

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXMesaCamera.h>
#include <JX3DWidget.h>
#include <J3DUniverse.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXWindowPainter.h>
#include <JXColormap.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXMesaCamera::JXMesaCamera
	(
	JX3DWidget*		widget,
	J3DUniverse*	universe
	)
	:
	J3DCamera(universe),
	J3DPainter(*(widget->GetColormap()))
{
	InitMesa(widget, universe);
}

JXMesaCamera::JXMesaCamera
	(
	JX3DWidget*		widget,
	J3DUniverse*	universe,
	const JVector&	position,
	const JVector&	attentionPt,
	const JVector&	upVector
	)
	:
	J3DCamera(universe, position, attentionPt, upVector),
	J3DPainter(*(widget->GetColormap()))
{
	InitMesa(widget, universe);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXMesaCamera::~JXMesaCamera()
{
	CloseMesa();
}

/******************************************************************************
 Render (virtual)

 ******************************************************************************/

void
JXMesaCamera::Render()
{
	if (!PrepareMesa())
		{
		return;
		}

	// erase everything

	SetBackColor(itsWidget->GetBackColor());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// prepare the display

	PrepareTransforms();

	// render the scene

	GetUniverse()->RenderAll(*this);

	// display the scene

	glFlush();

	if (itsDoubleBufferFlag)
		{
		XMesaCopySubBuffer(itsXMBuffer, itsGLViewport.x,itsGLViewport.y,
						   itsGLViewport.width,itsGLViewport.height);
		}
}

/******************************************************************************
 Refresh (virtual)

 ******************************************************************************/

void
JXMesaCamera::Refresh()
{
	itsWidget->Refresh();
}

/******************************************************************************
 Redraw (virtual)

 ******************************************************************************/

void
JXMesaCamera::Redraw()
{
	itsWidget->Redraw();
}

/******************************************************************************
 ModelToScreen (virtual)

 ******************************************************************************/

JVector
JXMesaCamera::ModelToScreen
	(
	const JVector& pt
	)
{
	if (!PrepareMesa())
		{
		return pt;
		}

	PrepareTransforms();

	GLdouble modelMatrix[16], projMatrix[16];
	GLint viewport[4];

	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, modelMatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);

	JFloat x,y,z;
	gluProject(pt.GetElement(1), pt.GetElement(2), pt.GetElement(3),
			   modelMatrix, projMatrix, viewport, &x, &y, &z);

	return JVector(3, x, y, z);
}

/******************************************************************************
 ScreenToModel (virtual)

 ******************************************************************************/

JVector
JXMesaCamera::ScreenToModel
	(
	const JVector& pt
	)
{
	if (!PrepareMesa())
		{
		return pt;
		}

	PrepareTransforms();

	GLdouble modelMatrix[16], projMatrix[16];
	GLint viewport[4];

	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);

	JFloat x,y,z;
	gluUnProject(pt.GetElement(1), pt.GetElement(2), pt.GetElement(3),
				 modelMatrix, projMatrix, viewport, &x, &y, &z);

	return JVector(3, x, y, z);
}

/******************************************************************************
 InitMesa (private)

	Mesa can handle RGBA mode with any colormap.

 ******************************************************************************/

void
JXMesaCamera::InitMesa
	(
	JX3DWidget*		widget,
	J3DUniverse*	universe
	)
{
	assert( widget->GetColormap() == &(universe->GetColormap()) );

	itsWidget           = widget;
	itsDoubleBufferFlag = kJTrue;

	itsPerspectiveFlag  = kJTrue;
	itsFOVAngle         = 45.0;
	itsNearZ            = 0.1;
	itsFarZ             = 10.0;

	XVisualInfo xvi = (itsWidget->GetColormap())->GetXVisualInfo();
	itsXMVisual =
		XMesaCreateVisual(*(itsWidget->GetDisplay()),
						  &xvi,
						  kJTrue,				// RGB
						  GL_FALSE,				// alpha buffer
						  itsDoubleBufferFlag,	// double buffering
						  GL_FALSE,				// stereo visual
						  GL_FALSE,				// use Pixmap for back buffer
						  16,					// bits/depth
						  0,					// bits/stencil
						  0,0,0,0,				// bits/component (accum)
						  0,					// number of samples/pixel
						  0,					// visual level
						  GLX_NONE_EXT);		// GLX extension
	assert( itsXMVisual != NULL );

	itsXMContext = XMesaCreateContext(itsXMVisual, NULL);
	assert( itsXMContext != NULL );

	itsXMBuffer =
		XMesaCreateWindowBuffer(itsXMVisual, (itsWidget->GetWindow())->GetXWindow());
	assert( itsXMBuffer != NULL );

	const JBoolean ok = PrepareMesa();
	assert( ok );

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	if (!itsDoubleBufferFlag)
		{
		glEnable(GL_SCISSOR_TEST);
		}
	glEnable(GL_DITHER);
	glEnable(GL_LINE_SMOOTH);		// anti-aliasing
//	glEnable(GL_POLYGON_SMOOTH);	// anti-aliasing

	UpdateViewport();
}

/******************************************************************************
 CloseMesa (private)

 ******************************************************************************/

void
JXMesaCamera::CloseMesa()
{
	XMesaDestroyBuffer(itsXMBuffer);
	XMesaDestroyContext(itsXMContext);
	XMesaDestroyVisual(itsXMVisual);
}

/******************************************************************************
 PrepareTransforms (private)

 ******************************************************************************/

void
JXMesaCamera::PrepareTransforms()
{
	// texture transform

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	// projection transform

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	const JFloat aspectRatio = itsGLViewport.width / (JFloat) itsGLViewport.height;
	if (itsPerspectiveFlag)
		{
		gluPerspective(itsFOVAngle, aspectRatio, itsNearZ, itsFarZ);
		}
	else
		{
		glOrtho(-1.0,+1.0,
				-1.0/aspectRatio,+1.0/aspectRatio,
				-1.0,+1.0);
		}

	// viewing transform -- must be last so objects can change it

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	const JVector& p = GetPosition();
	const JVector& a = GetAttentionPt();
	const JVector& u = GetUpVector();

	gluLookAt(p.GetElement(1), p.GetElement(2), p.GetElement(3),
			  a.GetElement(1), a.GetElement(2), a.GetElement(3),
			  u.GetElement(1), u.GetElement(2), u.GetElement(3));
}

/******************************************************************************
 UpdateViewport (virtual)

 ******************************************************************************/

void
JXMesaCamera::UpdateViewport()
{
	if (PrepareMesa())
		{
		const JRect bG       = itsWidget->GetBoundsGlobal();
		itsGLViewport.x      = bG.left;
		itsGLViewport.y      = (itsWidget->GetWindow())->GetBoundsHeight() - bG.bottom;
		itsGLViewport.width  = bG.width();
		itsGLViewport.height = bG.height();

		glViewport(itsGLViewport.x,itsGLViewport.y,
				   itsGLViewport.width,itsGLViewport.height);
		glScissor(itsGLViewport.x,itsGLViewport.y,
				  itsGLViewport.width,itsGLViewport.height);
		}
}

/******************************************************************************
 SetBackColor (private)

	Mesa can handle RGBA mode with any colormap.

 ******************************************************************************/

void
JXMesaCamera::SetBackColor
	(
	const JColorIndex color
	)
{
	if (!PrepareMesa())
		{
		return;
		}

	JSize r,g,b;
	(GetColormap()).GetRGB(color, &r, &g, &b);
	glClearColor(r/kJMaxRGBValueF,
				 g/kJMaxRGBValueF,
				 b/kJMaxRGBValueF,
				 0.0);
}

/******************************************************************************
 HWSetVertexColor (virtual protected)

	Mesa can handle RGBA mode with any colormap.

 ******************************************************************************/

void
JXMesaCamera::HWSetVertexColor
	(
	const JColormap&	colormap,
	const JColorIndex	color
	)
{
	JSize r,g,b;
	colormap.GetRGB(color, &r, &g, &b);
	glColor3f(r/kJMaxRGBValueF,
			  g/kJMaxRGBValueF,
			  b/kJMaxRGBValueF);
}
