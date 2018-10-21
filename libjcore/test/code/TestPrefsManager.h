/******************************************************************************
 TestPrefsManager.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestPrefsManager
#define _H_TestPrefsManager

#include <JPrefsManager.h>

class TestPrefsManager : public JPrefsManager
{
public:

	TestPrefsManager(const JUtf8Byte* signature, const JFileVersion currentVersion);

	virtual ~TestPrefsManager();

protected:

	virtual void	UpgradeData(const JBoolean isNew, const JFileVersion currentVersion) override;

private:

	// not allowed

	TestPrefsManager(const TestPrefsManager& source);
	const TestPrefsManager& operator=(const TestPrefsManager& source);
};

#endif
