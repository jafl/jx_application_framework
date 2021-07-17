/******************************************************************************
 CBFileNodeBase.h

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#ifndef _H_CBFileNodeBase
#define _H_CBFileNodeBase

#include "CBProjectNode.h"

class CBFileNodeBase : public CBProjectNode
{
public:

	CBFileNodeBase(CBProjectTree* tree, const CBProjectNodeType type,
				   const JString& fileName);
	CBFileNodeBase(std::istream& input, const JFileVersion vers,
				   CBProjectNode* parent, const CBProjectNodeType type);

	virtual ~CBFileNodeBase();

	const JString&	GetFileName() const;
	void			SetFileName(const JString& fileName);

	virtual bool	GetFullName(JString* fullName) const override;

	virtual void		StreamOut(std::ostream& output) const override;
	virtual bool	IncludedInMakefile() const override;
	virtual void		BuildMakeFiles(JString* text,
									   JPtrArray<JTreeNode>* invalidList,
									   JPtrArray<JString>* libFileList,
									   JPtrArray<JString>* libProjPathList) const override;
	virtual bool	IncludedInCMakeData() const override;
	virtual void		BuildCMakeData(JString* src, JString* hdr,
									   JPtrArray<JTreeNode>* invalidList) const override;
	virtual bool	IncludedInQMakeData() const override;
	virtual void		BuildQMakeData(JString* src, JString* hdr,
									   JPtrArray<JTreeNode>* invalidList) const override;
	virtual void		Print(JString* text) const override;
	virtual void		FileRenamed(const JString& origFullName, const JString& newFullName) override;
	virtual void		ShowFileLocation() const override;

	static CBFileNodeBase*	New(CBProjectTree* tree, const JString& fileName);

protected:

	bool	GetFullName(const JString& fileName, JString* fullName) const;
	void		ReportNotFound() const;

	virtual bool	FindFile1(const JString& fullName,
								  CBProjectNode** node) override;

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
