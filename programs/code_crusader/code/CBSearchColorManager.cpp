/******************************************************************************
 CBSearchColorManager.cpp

	BASE CLASS = JColorManager

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CBSearchColorManager.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBSearchColorManager::CBSearchColorManager()
	:
	JColorManager()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBSearchColorManager::~CBSearchColorManager()
{
}

/******************************************************************************
 GetColorID (virtual)

 ******************************************************************************/

bool
CBSearchColorManager::GetColorID
	(
	const JString&	name,
	JColorID*		id
	)
{
	*id = 0;
	return true;
}
