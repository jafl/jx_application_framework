/******************************************************************************
 JImageMask.h

	Interface for the JImageMask class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JImageMask
#define _H_JImageMask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>
#include <jColor.h>

class JImage;

class JImageMask
{
public:

	JImageMask();
	JImageMask(const JImageMask& source);

	virtual ~JImageMask();

	virtual JBoolean	ContainsPixel(const JCoordinate x, const JCoordinate y) const = 0;
	virtual void		AddPixel(const JCoordinate x, const JCoordinate y) = 0;
	virtual void		RemovePixel(const JCoordinate x, const JCoordinate y) = 0;

protected:

	void	CalcMask(const JImage& image, const JColorIndex color);

private:

	// not allowed

	const JImageMask& operator=(const JImageMask& source);
};

#endif
