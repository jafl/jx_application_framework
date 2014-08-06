/******************************************************************************
 GMChooseSavePrefObject.h

	Copyright © 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GMChooseSavePrefObject
#define _H_GMChooseSavePrefObject

#include <JPrefObject.h>

class GMChooseSaveFile;

class GMChooseSavePrefObject : public JPrefObject
{
public:

	GMChooseSavePrefObject(JPrefsManager* prefsMgr, const JPrefID& id, GMChooseSaveFile* csf);

	virtual ~GMChooseSavePrefObject();

protected:

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

private:

	GMChooseSaveFile*	itsCSF;

private:

	// not allowed

	GMChooseSavePrefObject(const GMChooseSavePrefObject& source);
	const GMChooseSavePrefObject& operator=(const GMChooseSavePrefObject& source);
};

#endif
