/******************************************************************************
 CBSearchColormap.h

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBSearchColormap
#define _H_CBSearchColormap

#include <JColorManager.h>

class CBSearchColormap : public JColorManager
{
public:

	CBSearchColormap();

	virtual ~CBSearchColormap();

	virtual JBoolean	GetColorID(const JString& name, JColorID* id) override;

private:

	// not allowed

	CBSearchColormap(const CBSearchColormap& source);
	const CBSearchColormap& operator=(const CBSearchColormap& source);
};

#endif
