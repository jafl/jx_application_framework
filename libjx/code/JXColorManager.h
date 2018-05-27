/******************************************************************************
 JXColorManager.h

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXColorManager
#define _H_JXColorManager

#include <JColorManager.h>
#include <JBroadcaster.h>
#include <jXConstants.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

class JXDisplay;

class JXColorManager : public JColorManager, public virtual JBroadcaster
{
	friend class JXDisplay;

public:

	virtual ~JXColorManager();

	operator Colormap() const;

	JXDisplay*	GetDisplay();
	Colormap	GetXColormap();
	XVisualInfo	GetXVisualInfo();
	Visual*		GetVisual();
	int			GetVisualClass();

	virtual JBoolean	GetColorID(const JString& name, JColorID* id);
	unsigned long		GetXColor(const JColorID id);

private:

	JXDisplay*	itsDisplay;		// not owned
	Colormap	itsXColormap;	// not owned
	Visual*		itsVisual;		// not owned
	XVisualInfo	itsXVisualInfo;

	// for calculating preallocated X pixel values

	unsigned long	itsMask[3];
	short			itsStartIndex[3];
	short			itsEndIndex[3];

private:

	JXColorManager(JXDisplay* display, Visual* visual, Colormap xColormap);

	void	InitMasks(const unsigned long redMask,
					  const unsigned long greenMask,
					  const unsigned long blueMask);

	// called by JXDisplay

	static JXColorManager*	Create(JXDisplay* display);

	// not allowed

	JXColorManager(const JXColorManager& source);
	const JXColorManager& operator=(const JXColorManager& source);
};


/******************************************************************************
 GetDisplay

 ******************************************************************************/

inline JXDisplay*
JXColorManager::GetDisplay()
{
	return itsDisplay;
}

/******************************************************************************
 Cast to Colormap

 ******************************************************************************/

inline
JXColorManager::operator Colormap()
	const
{
	return itsXColormap;
}

/******************************************************************************
 GetXColormap

 ******************************************************************************/

inline Colormap
JXColorManager::GetXColormap()
{
	return itsXColormap;
}

/******************************************************************************
 GetXVisualInfo

	Nobody wants a "const XVisualInfo*", but it is too dangerous to
	return a non-const pointer to our data.

 ******************************************************************************/

inline XVisualInfo
JXColorManager::GetXVisualInfo()
{
	return itsXVisualInfo;
}

/******************************************************************************
 GetVisual

 ******************************************************************************/

inline Visual*
JXColorManager::GetVisual()
{
	return itsVisual;
}

/******************************************************************************
 GetVisualClass

 ******************************************************************************/

inline int
JXColorManager::GetVisualClass()
{
	return itsXVisualInfo.c_class;
}

#endif
