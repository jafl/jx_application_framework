/******************************************************************************
 CBFileNodeBase.h

	Copyright (C) 1999 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBFileNodeBase
#define _H_CBFileNodeBase

#include "CBProjectNode.h"

class CBFileNodeBase : public CBProjectNode
{
public:

	CBFileNodeBase(CBProjectTree* tree, const CBProjectNodeType type,
				   const JCharacter* fileName);
	CBFileNodeBase(istream& input, const JFileVersion vers,
				   CBProjectNode* parent, const CBProjectNodeType type);

	virtual ~CBFileNodeBase();

	const JString&	GetFileName() const;
	void			SetFileName(const JCharacter* fileName);

	virtual JBoolean	GetFullName(JString* fullName) const;

	virtual void		StreamOut(ostream& output) const;
	virtual JBoolean	IncludedInMakefile() const;
	virtual void		BuildMakeFiles(JString* text,
									   JPtrArray<JTreeNode>* invalidList,
									   JPtrArray<JString>* libFileList,
									   JPtrArray<JString>* libProjPathList) const;
	virtual JBoolean	IncludedInCMakeData() const;
	virtual void		BuildCMakeData(JString* src, JString* hdr,
									   JPtrArray<JTreeNode>* invalidList) const;
	virtual JBoolean	IncludedInQMakeData() const;
	virtual void		BuildQMakeData(JString* src, JString* hdr,
									   JPtrArray<JTreeNode>* invalidList) const;
	virtual void		Print(JString* text) const;
	virtual void		FileRenamed(const JCharacter* origFullName, const JCharacter* newFullName);
	virtual void		ShowFileLocation() const;

	static CBFileNodeBase*	New(CBProjectTree* tree, const JString& fileName);

protected:

	JBoolean	GetFullName(const JCharacter* fileName, JString* fullName) const;
	void		ReportNotFound() const;

	virtual JBoolean	FindFile1(const JCharacter* fullName,
								  CBProjectNode** node);

private:

	JString	itsFileName;	// can be relative

private:

	// not allowed

	CBFileNodeBase(const CBFileNodeBase& source);
	CBFileNodeBase& operator=(const CBFileNodeBase& source);
};


/******************************************************************************
 GetFileName

	This can be a relative path.

 ******************************************************************************/

inline const JString&
CBFileNodeBase::GetFileName()
	const
{
	return itsFileName;
}

#endif
