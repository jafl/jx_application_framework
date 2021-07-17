/******************************************************************************
 CBRPChooseFileDialog.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBRPChooseFileDialog
#define _H_CBRPChooseFileDialog

#include <JXChooseFileDialog.h>
#include "CBRelPathCSF.h"		// need definition of PathType

class JXRadioGroup;

class CBRPChooseFileDialog : public JXChooseFileDialog
{
public:

	static CBRPChooseFileDialog*
		Create(JXDirector* supervisor, JDirInfo* dirInfo,
			   const JString& fileFilter,
			   const bool allowSelectMultiple,
			   const CBRelPathCSF::PathType pathType,
			   const JString& origName, const JString& message);

	virtual ~CBRPChooseFileDialog();

	CBRelPathCSF::PathType	GetPathType() const;

protected:

	CBRPChooseFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						 const JString& fileFilter,
						 const bool allowSelectMultiple);

private:

// begin JXLayout

	JXRadioGroup* itsPathTypeRG;

// end JXLayout

private:

	void	BuildWindow(const CBRelPathCSF::PathType pathType,
						const JString& origName,
						const JString& message);

	// not allowed

	CBRPChooseFileDialog(const CBRPChooseFileDialog& source);
	const CBRPChooseFileDialog& operator=(const CBRPChooseFileDialog& source);
};

#endif
