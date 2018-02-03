/******************************************************************************
 CBCommandMenu.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBCommandMenu
#define _H_CBCommandMenu

#include <JXTextMenu.h>

class CBProjectDocument;
class CBTextDocument;

class CBCommandMenu : public JXTextMenu
{
public:

	class GetTargetInfo;

public:

	CBCommandMenu(CBProjectDocument* projDoc, CBTextDocument* textDoc,
				  JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	CBCommandMenu(CBProjectDocument* projDoc, CBTextDocument* textDoc,
				  JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	void	SetProjectDocument(CBProjectDocument* projDoc);

	virtual ~CBCommandMenu();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CBProjectDocument*	itsProjDoc;			// not owned; NULL if should use active project
	CBTextDocument*		itsTextDoc;			// not owned; NULL if not in menu bar of text doc
	JXTextMenu*			itsAddToProjMenu;	// not owned; NULL if itsTextDoc == NULL
	JXTextMenu*			itsManageProjMenu;	// not owned; NULL if itsTextDoc == NULL

private:

	void	CBCommandMenuX(CBProjectDocument* projDoc, CBTextDocument* textDoc);

	void	UpdateMenu();
	void	HandleSelection(const JIndex index, const GetTargetInfo& info);

	void	UpdateAddToProjectMenu();
	void	HandleAddToProjectMenu(const JIndex index);

	JBoolean	CanAddToProject() const;

	void	UpdateManageProjectMenu();
	void	HandleManageProjectMenu(const JIndex index);

	JBoolean	CanManageProject() const;

	void	BuildTargetInfo(GetTargetInfo* info);

	// not allowed

	CBCommandMenu(const CBCommandMenu& source);
	const CBCommandMenu& operator=(const CBCommandMenu& source);

public:

	// JBroadcaster messages

	static const JCharacter* kGetTargetInfo;

	class GetTargetInfo : public JBroadcaster::Message
		{
		public:

			GetTargetInfo()
				:
				JBroadcaster::Message(kGetTargetInfo),
				itsFileList(JPtrArrayT::kDeleteAll)
				{ };

			JBoolean
			HasFiles() const
			{
				return !itsFileList.IsEmpty();
			};

			JPtrArray<JString>*
			GetFileList()
			{
				return &itsFileList;
			};

			const JPtrArray<JString>&
			GetFileList() const
			{
				return itsFileList;
			};

			JArray<JIndex>*
			GetLineIndexList()
			{
				return &itsLineIndexList;
			};

			const JArray<JIndex>&
			GetLineIndexList() const
			{
				return itsLineIndexList;
			};

			void
			AddFile(const JCharacter* fullName, const JIndex lineIndex = 0)
			{
				itsFileList.Append(fullName);
				itsLineIndexList.AppendElement(lineIndex);
			};

		private:

			JPtrArray<JString>	itsFileList;
			JArray<JIndex>		itsLineIndexList;	// can be empty
		};
};

#endif
