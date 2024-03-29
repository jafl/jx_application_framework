/******************************************************************************
 TestPrefObject.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestPrefObject
#define _H_TestPrefObject

#include "JPrefObject.h"

class TestPrefObject : public JPrefObject
{
public:

	TestPrefObject(JPrefsManager* prefsMgr, const JPrefID& prefID);

	~TestPrefObject() override;

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

private:

	JIndex	itsNumber;
};

#endif
