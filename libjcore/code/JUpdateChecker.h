/******************************************************************************
 JUpdateChecker.h

	Copyright (C) 2010-22 by John Lindal.

 ******************************************************************************/

#ifndef _H_JUpdateChecker
#define _H_JUpdateChecker

#include "jx-af/jcore/JPrefObject.h"
#include "jx-af/jcore/JString.h"

class JUpdateChecker : public JPrefObject
{
public:

	JUpdateChecker(JPrefsManager* prefsMgr, const JPrefID& prefID);

	~JUpdateChecker() override;

	const JString&	GetLatestVersion() const;
	bool			TimeToRemind();

	void	CheckForNewerVersion();

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

private:

	JPrefID			itsPrefID;
	JString			itsVersion;
	time_t			itsNextServerTime;
	JArray<time_t>*	itsReminderList;
};


/******************************************************************************
 GetLatestVersion

 ******************************************************************************/

inline const JString&
JUpdateChecker::GetLatestVersion()
	const
{
	return itsVersion;
}

#endif
