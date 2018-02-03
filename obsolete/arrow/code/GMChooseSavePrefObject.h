/******************************************************************************
 GMChooseSavePrefObject.h

	Copyright (C) 1999 by Glenn W. Bach.

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

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;

private:

	GMChooseSaveFile*	itsCSF;

private:

	// not allowed

	GMChooseSavePrefObject(const GMChooseSavePrefObject& source);
	const GMChooseSavePrefObject& operator=(const GMChooseSavePrefObject& source);
};

#endif
