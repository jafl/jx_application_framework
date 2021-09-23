/******************************************************************************
 JImageMask.h

	Interface for the JImageMask class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JImageMask
#define _H_JImageMask

#include "jTypes.h"
#include "jColor.h"

class JImage;

class JImageMask
{
public:

	JImageMask();
	JImageMask(const JImageMask& source);

	virtual ~JImageMask();

	virtual bool	ContainsPixel(const JCoordinate x, const JCoordinate y) const = 0;
	virtual void		AddPixel(const JCoordinate x, const JCoordinate y) = 0;
	virtual void		RemovePixel(const JCoordinate x, const JCoordinate y) = 0;

protected:

	void	CalcMask(const JImage& image, const JColorID color);

private:

	// not allowed

	JImageMask& operator=(const JImageMask&) = delete;
};

#endif
