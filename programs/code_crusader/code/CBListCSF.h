/******************************************************************************
 CBListCSF.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBListChooseSaveFile
#define _H_CBListChooseSaveFile

#include <JXChooseSaveFile.h>

class CBListChooseFileDialog;

class CBListCSF : public JXChooseSaveFile
{
public:

	CBListCSF(const JString& replaceListStr,
			  const JString& appendToListStr);

	virtual ~CBListCSF();

	JBoolean	ReplaceExisting() const;

protected:

	virtual JXChooseFileDialog*
	CreateChooseFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						   const JString& fileFilter,
						   const JBoolean allowSelectMultiple,
						   const JString& origName, const JString& message) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CBListChooseFileDialog*	itsChooseDialog;
	JBoolean				itsReplaceExistingFlag;
	JString					itsReplaceListStr;
	JString					itsAppendToListStr;

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
