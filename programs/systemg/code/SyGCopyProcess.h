/******************************************************************************
 SyGCopyProcess.h

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_SyGCopyProcess
#define _H_SyGCopyProcess

#include <JPtrArray-JString.h>
#include <jVCSUtil.h>

class JSimpleProcess;
class SyGFileTreeTable;
class SyGFileTreeNode;

class SyGCopyProcess : virtual public JBroadcaster
{
public:

	static void	Copy(SyGFileTreeTable* srcTable, JPtrArray<JString>* srcNameList,
					 SyGFileTreeTable* destTable, SyGFileTreeNode* destNode);
	static void	Move(SyGFileTreeTable* srcTable, JPtrArray<JString>* srcNameList,
					 SyGFileTreeTable* destTable, SyGFileTreeNode* destNode);

	virtual ~SyGCopyProcess();

protected:

	SyGCopyProcess(SyGFileTreeTable* srcTable, JPtrArray<JString>* srcNameList,
				   SyGFileTreeTable* destTable, SyGFileTreeNode* destNode,
				   const JBoolean isCopy);

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	SyGFileTreeTable*			itsSrcTable;		// not owned; can be nullptr
	JPtrArray<JString>*			itsSrcNameList;		// usually from DND data
	SyGFileTreeTable*			itsDestTable;		// not owned; can be nullptr
	SyGFileTreeNode*			itsDestNode;		// not owned; can be nullptr
	JVCSType					itsVCSType;
	const JBoolean				itsIsMoveFlag;
	JSimpleProcess*				itsProcess;

private:

	void	Start(const JSize prefixCount);
	void	RemoveExecutePermissions(const JString& srcPath,
									 const JString& destPath);

	static JBoolean	CleanSrcList(JPtrArray<JString>* srcNameList,
								 SyGFileTreeNode* destNode);
	static JBoolean	ActionIsUseful(const JString& srcName, const JString& destPath,
								   JString* destName);
	static JBoolean	OKToReplace(const JString& srcName, const JString& destName,
								JBoolean* ask, JBoolean* first);

	// not allowed

	SyGCopyProcess(const SyGCopyProcess& source);
	const SyGCopyProcess& operator=(const SyGCopyProcess& source);
};

#endif
