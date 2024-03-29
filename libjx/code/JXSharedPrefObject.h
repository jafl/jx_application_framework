/******************************************************************************
 JXSharedPrefObject.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXSharedPrefObject
#define _H_JXSharedPrefObject

#include <jx-af/jcore/JPrefsManager.h>	// for JPrefID

class JXSharedPrefObject : virtual public JBroadcaster
{
public:

	struct VersionInfo
	{
		JFileVersion	vers;
		JPrefID			id;

		VersionInfo()
			:
			vers(0), id(0)
			{ };

		VersionInfo(const JFileVersion v, const JPrefID& i)
			:
			vers(v), id(i)
			{ };
	};

public:

	JXSharedPrefObject(const JFileVersion currVers, const JPrefID& latestVersID,
					   const VersionInfo versList[], const JSize versCount);

	~JXSharedPrefObject() override;

	JFileVersion				GetCurrentPrefsVersion() const;
	const JPrefID&				GetLatestVersionID() const;
	JPrefID						GetPrefID(const JFileVersion vers) const;
	const JArray<VersionInfo>&	GetVersionList() const;

	void	ReadPrefs();
	void	WritePrefs() const;

	// override only in leaf classes

	virtual void	ReadPrefs(std::istream& input) = 0;
	virtual void	WritePrefs(std::ostream& output, const JFileVersion vers) const = 0;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JFileVersion			itsCurrentPrefsVersion;
	JPrefID					itsLatestVersionID;
	JArray<VersionInfo>*	itsVersionList;

private:

	// not allowed

	JXSharedPrefObject(const JXSharedPrefObject&) = delete;
	JXSharedPrefObject& operator=(const JXSharedPrefObject&) = delete;
};


/******************************************************************************
 GetCurrentPrefsVersion

 ******************************************************************************/

inline JFileVersion
JXSharedPrefObject::GetCurrentPrefsVersion()
	const
{
	return itsCurrentPrefsVersion;
}

/******************************************************************************
 GetLatestVersionID

 ******************************************************************************/

inline const JPrefID&
JXSharedPrefObject::GetLatestVersionID()
	const
{
	return itsLatestVersionID;
}

/******************************************************************************
 GetVersionList

 ******************************************************************************/

inline const JArray<JXSharedPrefObject::VersionInfo>&
JXSharedPrefObject::GetVersionList()
	const
{
	return *itsVersionList;
}

#endif
