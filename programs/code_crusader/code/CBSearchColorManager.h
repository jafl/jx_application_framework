/******************************************************************************
 CBSearchColorManager.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBSearchColorManager
#define _H_CBSearchColorManager

#include <JColorManager.h>

class CBSearchColorManager : public JColorManager
{
public:

	CBSearchColorManager();

	virtual ~CBSearchColorManager();

	virtual bool	GetColorID(const JString& name, JColorID* id) override;

private:

	// not allowed

	CBSearchColorManager(const CBSearchColorManager& source);
	const CBSearchColorManager& operator=(const CBSearchColorManager& source);
};

#endif
