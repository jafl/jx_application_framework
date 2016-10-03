/******************************************************************************
 JXMesaCamera.h

	Interface for the JXMesaCamera class

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXMesaCamera
#define _H_JXMesaCamera

#include <J3DCamera.h>
#include <J3DPainter.h>
#include <GL/xmesa.h>

class JX3DWidget;

class JXMesaCamera : public J3DCamera, public J3DPainter
{
public:

	JXMesaCamera(JX3DWidget* widget, J3DUniverse* universe);
	JXMesaCamera(JX3DWidget* widget, J3DUniverse* universe,
				 const JVector& position, const JVector& attentionPt,
				 const JVector& upVector);

	virtual ~JXMesaCamera();

	virtual void	Render();
	virtual void	Refresh();
	virtual void	Redraw();

	JFloat	GetFOVAngle() const;
	void	SetFOVAngle(const JFloat fov);

	JFloat	GetNearZ() const;
	void	SetNearZ(const JFloat z);

	JFloat	GetFarZ() const;
	void	SetFarZ(const JFloat z);

	virtual JVector	ModelToScreen(const JVector& pt);
	virtual JVector	ScreenToModel(const JVector& pt);

	// called by JX3DWidget

	virtual void	UpdateViewport();

protected:

	virtual void	HWSetVertexColor(const JColormap& colormap,
									 const JColorIndex color);

private:

	XMesaVisual		itsXMVisual;
	XMesaContext	itsXMContext;
	XMesaBuffer		itsXMBuffer;
	XRectangle		itsGLViewport;
	JBoolean		itsDoubleBufferFlag;

	JBoolean		itsPerspectiveFlag;
	JFloat			itsFOVAngle;
	JFloat			itsNearZ;
	JFloat			itsFarZ;

	JX3DWidget*		itsWidget;		// owns us

private:

	void		InitMesa(JX3DWidget* widget, J3DUniverse* universe);
	void		CloseMesa();
	JBoolean	PrepareMesa();
	void		PrepareTransforms();

	void	SetBackColor(const JColorIndex color);

	// not allowed

	JXMesaCamera(const JXMesaCamera& source);
	const JXMesaCamera& operator=(const JXMesaCamera& source);
};


/******************************************************************************
 Field-of-view angle (degrees)

 ******************************************************************************/

inline JFloat
JXMesaCamera::GetFOVAngle()
	const
{
	return itsFOVAngle;
}

inline void
JXMesaCamera::SetFOVAngle
	(
	const JFloat fov
	)
{
	itsFOVAngle = fov;
	Refresh();
}

/******************************************************************************
 Near clipping plane

 ******************************************************************************/

inline JFloat
JXMesaCamera::GetNearZ()
	const
{
	return itsNearZ;
}

inline void
JXMesaCamera::SetNearZ
	(
	const JFloat z
	)
{
	itsNearZ = z;
	Refresh();
}

/******************************************************************************
 Far clipping plane

 ******************************************************************************/

inline JFloat
JXMesaCamera::GetFarZ()
	const
{
	return itsFarZ;
}

inline void
JXMesaCamera::SetFarZ
	(
	const JFloat z
	)
{
	itsFarZ = z;
	Refresh();
}

/******************************************************************************
 PrepareMesa (private)

 ******************************************************************************/

inline JBoolean
JXMesaCamera::PrepareMesa()
{
	return JI2B( XMesaMakeCurrent(itsXMContext, itsXMBuffer) );
}

#endif
