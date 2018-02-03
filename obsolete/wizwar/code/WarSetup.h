/******************************************************************************
 WarSetup.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_WarSetup
#define _H_WarSetup

#include <jTypes.h>

class WarSetup
{
public:

	WarSetup(const JSize argc, char* argv[], const JCharacter* versionStr);

	~WarSetup();

	JIndex		GetServerPort() const;
	JSize		GetMaxPlayerCount() const;
	JBoolean	GameIsAnonymous() const;

private:

	enum FileStatus
	{
		kFileReadable,
		kNeedNewerVersion,
		kNotMyFile
	};

private:

	JIndex		itsServerPort;
	JSize		itsMaxPlayerCount;
	JBoolean	itsIsAnonymousFlag;

private:

	void		Configure(const JSize argc, char* argv[], const JCharacter* versionStr);
	FileStatus	ReadSetup(const JCharacter* fileName);
	void		WriteSetup(const JCharacter* fileName) const;

	void	PrintHelp(const JCharacter* versionStr) const;
	void	PrintVersion(const JCharacter* versionStr) const;

	// not allowed

	WarSetup(const WarSetup& source);
	WarSetup& operator=(const WarSetup& source);
};


/******************************************************************************
 GetServerPort

 ******************************************************************************/

inline JIndex
WarSetup::GetServerPort()
	const
{
	return itsServerPort;
}

/******************************************************************************
 GetMaxPlayerCount

 ******************************************************************************/

inline JSize
WarSetup::GetMaxPlayerCount()
	const
{
	return itsMaxPlayerCount;
}

/******************************************************************************
 GameIsAnonymous

 ******************************************************************************/

inline JBoolean
WarSetup::GameIsAnonymous()
	const
{
	return itsIsAnonymousFlag;
}

#endif
