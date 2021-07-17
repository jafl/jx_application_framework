/******************************************************************************
 CBListChooseFileDialog.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBListChooseFileDialog
#define _H_CBListChooseFileDialog

#include <JXChooseFileDialog.h>

class JXRadioGroup;

class CBListChooseFileDialog : public JXChooseFileDialog
{
public:

	static CBListChooseFileDialog*
		Create(JXDirector* supervisor, JDirInfo* dirInfo,
			   const JString& fileFilter,
			   const bool allowSelectMultiple,
			   const JString& replaceListStr, const JString& appendToListStr,
			   const JString& origName, const JString& message);

	virtual ~CBListChooseFileDialog();

	bool	ReplaceExisting() const;

protected:

	CBListChooseFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						   const JString& fileFilter,
						   const bool allowSelectMultiple);

private:

// begin JXLayout

	JXRadioGroup* itsAppendReplaceRG;

// end JXLayout

private:

	void	BuildWindow(const JString& replaceListStr,
						const JString& appendToListStr,
						const JString& origName,
						const JString& message);

	// not allowed

	CBListChooseFileDialog(const CBListChooseFileDialog& source);
	const CBListChooseFileDialog& operator=(const CBListChooseFileDialog& source);
};

#endif
