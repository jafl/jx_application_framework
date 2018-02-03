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

	SyGFileTreeTable*			itsSrcTable;		// not owned; can be NULL
	JPtrArray<JString>*			itsSrcNameList;		// usually from DND data
	SyGFileTreeTable*			itsDestTable;		// not owned; can be NULL
	SyGFileTreeNode*			itsDestNode;		// not owned; can be NULL
	JVCSType					itsVCSType;
	const JBoolean				itsIsMoveFlag;
	JSimpleProcess*				itsProcess;

private:

	void	Start(const JSize prefixCount);
	void	RemoveExecutePermissions(const JCharacter* srcPath,
									 const JCharacter* destPath);

	static JBoolean	CleanSrcList(JPtrArray<JString>* srcNameList,
								 SyGFileTreeNode* destNode);
	static JBoolean	ActionIsUseful(const JCharacter* srcName, const JCharacter* destPath,
								   JString* destName);
	static JBoolean	OKToReplace(const JCharacter* srcName, const JCharacter* destName,
								JBoolean* ask, JBoolean* first);

	// not allowed

	SyGCopyProcess(const SyGCopyProcess& source);
	const SyGCopyProcess& operator=(const SyGCopyProcess& source);
};

#endif
