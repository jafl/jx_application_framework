/******************************************************************************
 CBListCSF.h

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBListChooseSaveFile
#define _H_CBListChooseSaveFile

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXChooseSaveFile.h>

class CBListChooseFileDialog;

class CBListCSF : public JXChooseSaveFile
{
public:

	CBListCSF(const JCharacter* replaceListStr,
			  const JCharacter* appendToListStr);

	virtual ~CBListCSF();

	JBoolean	ReplaceExisting() const;

protected:

	virtual JXChooseFileDialog*
	CreateChooseFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						   const JCharacter* fileFilter,
						   const JBoolean allowSelectMultiple,
						   const JCharacter* origName, const JCharacter* message);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	CBListChooseFileDialog*	itsChooseDialog;
	JBoolean				itsReplaceExistingFlag;
	const JCharacter*		itsReplaceListStr;
	const JCharacter*		itsAppendToListStr;

private:

	// not allowed

	CBListCSF(const CBListCSF& source);
	const CBListCSF& operator=(const CBListCSF& source);
};


/******************************************************************************
 ReplaceExisting

 ******************************************************************************/

inline JBoolean
CBListCSF::ReplaceExisting()
	const
{
	return itsReplaceExistingFlag;
}

#endif
