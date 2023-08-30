/******************************************************************************
 TestPrefsManager.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestPrefsManager
#define _H_TestPrefsManager

#include "JPrefsManager.h"

class TestPrefsManager : public JPrefsManager
{
public:

	TestPrefsManager(const JUtf8Byte* signature, const JFileVersion currentVersion);

	~TestPrefsManager() override;

protected:

	void	UpgradeData(const bool isNew, const JFileVersion currentVersion) override;
	void	DataLoaded() override;
};

#endif
