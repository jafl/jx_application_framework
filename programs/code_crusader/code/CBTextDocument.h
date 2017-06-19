/******************************************************************************
 CBTextDocument.h

	Interface for the CBTextDocument class

	Copyright (C) 1996-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBTextDocument
#define _H_CBTextDocument

#include <JXFileDocument.h>
#include <JPrefObject.h>
#include "CBTextFileType.h"
#include <JXWidget.h>		// need defn of H/VSizing
#include <JTextEditor.h>	// need defn of PlainTextFormat, CRMRuleList

class JTEStyler;
class CBTextEditor;
class CBFileDragSource;
class CBFileNameDisplay;
class CBStylerBase;
class CBStringCompleter;
class CBCharActionManager;
class CBMacroManager;
class CBTabWidthDialog;
class CBCommandMenu;

class JXWidget;
class JXTextButton;
class JXTextCheckbox;
class JXMenuBar;
class JXTextMenu;
class JXToolBar;

class CBTextDocument;
class CBTELineIndexInput;
class CBTEColIndexInput;
class JXScrollbarSet;
typedef CBTextEditor* (CBTextEditorCtorFn)(CBTextDocument* document,
											const JCharacter* fileName,
											JXMenuBar* menuBar,
											CBTELineIndexInput* lineInput,
											CBTEColIndexInput* colInput,
											JXScrollbarSet* scrollbarSet);

class CBTextDocument : public JXFileDocument, public JPrefObject
{
public:

	CBTextDocument(const CBTextFileType type = kCBUnknownFT,
				   const JCharacter* helpSectionName = "CBEditorHelp",
				   const JBoolean setWMClass = kJTrue,
				   CBTextEditorCtorFn* teCtorFn = ConstructTextEditor);
	CBTextDocument(const JCharacter* fileName,
				   const CBTextFileType type = kCBUnknownFT,
				   const JBoolean tmpl = kJFalse);
	CBTextDocument(std::istream& input, const JFileVersion vers, JBoolean* keep);

	virtual ~CBTextDocument();

	virtual void		Activate();
	virtual JBoolean	GetMenuIcon(const JXImage** icon) const;

	void	GoToLine(const JIndex lineIndex) const;
	void	SelectLines(const JIndexRange& range) const;
	void	EditPrefs();
	void	HandleActionButton();

	void	WriteForProject(std::ostream& output) const;

	JBoolean	GetWindowSize(JPoint* desktopLoc,
							  JCoordinate* width, JCoordinate* height) const;
	void		SaveWindowSize() const;

	JBoolean	WillOpenComplFileOnTop() const;
	void		ShouldOpenComplFileOnTop(const JBoolean doIt);

	CBTextEditor*	GetTextEditor() const;
	JXMenuBar*		GetMenuBar() const;
	JXToolBar*		GetToolBar() const;
	JRect			GetFileDisplayInfo(JXWidget::HSizingOption* hSizing,
									   JXWidget::VSizingOption* vSizing) const;
	void			SetFileDisplayVisible(const JBoolean show);

	CBTextFileType	GetFileType() const;
	JBoolean		GetStyler(CBStylerBase** styler) const;
	JBoolean		GetStringCompleter(CBStringCompleter** completer) const;
	JBoolean		GetCharActionManager(CBCharActionManager** mgr) const;
	JBoolean		GetMacroManager(CBMacroManager** mgr) const;

	void	OverrideTabWidth(const JSize tabWidth);
	void	OverrideAutoIndent(const JBoolean autoIndent);
	void	OverrideTabInsertsSpaces(const JBoolean insertSpaces);
	void	OverrideShowWhitespace(const JBoolean showWhitespace);
	void	OverrideBreakCROnly(const JBoolean breakCROnly);
	void	OverrideReadOnly(const JBoolean readOnly);

	static void	ReadStaticGlobalPrefs(std::istream& input, const JFileVersion vers);
	static void	WriteStaticGlobalPrefs(std::ostream& output);

	// called by CBDocumentManager

	void	UpdateFileType(const JBoolean init = kJFalse);
	void	StylerChanged(JTEStyler* styler);

	void	RefreshCVSStatus();

	static JBoolean	OpenAsBinaryFile(const JCharacter* fileName);

	// called by CBTextEditor

	virtual void	ConvertSelectionToFullPath(JString* fileName) const;

protected:

	JXTextMenu*		InsertTextMenu(const JCharacter* title);
	CBCommandMenu*	GetCommandMenu();

	void			ReadFile(const JCharacter* fileName, const JBoolean firstTime);
	virtual void	WriteTextFile(std::ostream& output, const JBoolean safetySave) const;
	virtual void	DiscardChanges();

	virtual void	HandleFileModifiedByOthers(const JBoolean modTimeChanged,
											   const JBoolean permsChanged);

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	enum
	{
		kAutoIndentIndex = 0,
		kTabInsertsSpacesIndex,
		kShowWhitespaceIndex,
		kWordWrapIndex,
		kReadOnlyIndex,
		kTabWidthIndex,

		kSettingCount
	};

private:

	CBTextEditor*		itsTextEditor;				// owned by its enclosure
	JBoolean			itsOpenOverComplementFlag;	// kJTrue if open window over complement file
	JXTextMenu*			itsFileMenu;
	JXTextMenu*			itsFileFormatMenu;
	JXTextMenu*			itsDiffMenu;
	CBCommandMenu*		itsCmdMenu;
	JXTextMenu*			itsWindowMenu;
	JXTextMenu*			itsPrefsMenu;
	JXTextMenu*			itsPrefsStylesMenu;
	JXTextMenu*			itsHelpMenu;

	CBTextFileType				itsFileType;
	CBCharActionManager*		itsActionMgr;		// not owned; can be NULL
	CBMacroManager*				itsMacroMgr;		// not owned; can be NULL
	JTextEditor::CRMRuleList*	itsCRMRuleList;		// not owned; can be NULL

	const JCharacter*	itsHelpSectionName;
	JBoolean			itsUpdateFileTypeFlag;		// kJFalse while constructing
	JBoolean			itsOverrideFlag[ kSettingCount ];

	JTextEditor::PlainTextFormat	itsFileFormat;

	CBTabWidthDialog*	itsTabWidthDialog;

// begin JXLayout

	CBFileDragSource*  itsFileDragSource;
	JXMenuBar*         itsMenuBar;
	JXTextButton*      itsActionButton;
	JXToolBar*         itsToolBar;
	CBFileNameDisplay* itsFileDisplay;
	JXTextMenu*        itsSettingsMenu;

// end JXLayout

private:

	void	CBTextDocumentX1(const CBTextFileType type);
	void	CBTextDocumentX2(const JBoolean setWindowSize);
	void	BuildWindow(const JBoolean setWMClass,
						CBTextEditorCtorFn* teCtorFn);
	void	DisplayFileName(const JCharacter* name);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex item);

	JBoolean	OpenSomething();

	void	UpdateFileFormatMenu();
	void	HandleFileFormatMenu(const JIndex item);

	void	UpdateDiffMenu();
	void	HandleDiffMenu(const JIndex item);

	void	UpdatePrefsMenu();
	void	HandlePrefsMenu(const JIndex index);

	void	UpdatePrefsStylesMenu();
	void	HandlePrefsStylesMenu(const JIndex index);

	void	UpdateSettingsMenu();
	void	HandleSettingsMenu(const JIndex index);

	JBoolean	ReadFromProject(std::istream& input, const JFileVersion vers);

	void	UpdateReadOnlyDisplay();

	static CBTextEditor*	ConstructTextEditor(CBTextDocument* document,
												const JCharacter* fileName,
												JXMenuBar* menuBar,
												CBTELineIndexInput* lineInput,
												CBTEColIndexInput* colInput,
												JXScrollbarSet* scrollbarSet);

	// not allowed

	CBTextDocument(const CBTextDocument& source);
	const CBTextDocument& operator=(const CBTextDocument& source);

public:

	// JBroadcaster messages

	static const JCharacter* kSaved;

	class Saved : public JBroadcaster::Message
		{
		public:

			Saved()
				:
				JBroadcaster::Message(kSaved)
				{ };
		};
};


/******************************************************************************
 Placement of complement file

 ******************************************************************************/

inline JBoolean
CBTextDocument::WillOpenComplFileOnTop()
	const
{
	return itsOpenOverComplementFlag;
}

inline void
CBTextDocument::ShouldOpenComplFileOnTop
	(
	const JBoolean doIt
	)
{
	itsOpenOverComplementFlag = doIt;
}

/******************************************************************************
 GetTextEditor

 ******************************************************************************/

inline CBTextEditor*
CBTextDocument::GetTextEditor()
	const
{
	return itsTextEditor;
}

/******************************************************************************
 GetMenuBar

 ******************************************************************************/

inline JXMenuBar*
CBTextDocument::GetMenuBar()
	const
{
	return itsMenuBar;
}

/******************************************************************************
 File type

 ******************************************************************************/

inline CBTextFileType
CBTextDocument::GetFileType()
	const
{
	return itsFileType;
}

/******************************************************************************
 GetStringCompleter

	Returns the CBStringCompleter to be used for this document, if any.

 ******************************************************************************/

inline JBoolean
CBTextDocument::GetStringCompleter
	(
	CBStringCompleter** completer
	)
	const
{
	return CBGetCompleter(itsFileType, completer);
}

/******************************************************************************
 GetCharActionManager

	Returns the CBCharActionManager to be used for this document, if any.

 ******************************************************************************/

inline JBoolean
CBTextDocument::GetCharActionManager
	(
	CBCharActionManager** mgr
	)
	const
{
	*mgr = itsActionMgr;
	return JI2B( itsActionMgr != NULL );
}

/******************************************************************************
 GetMacroManager

	Returns the CBMacroManager to be used for this document, if any.

 ******************************************************************************/

inline JBoolean
CBTextDocument::GetMacroManager
	(
	CBMacroManager** mgr
	)
	const
{
	*mgr = itsMacroMgr;
	return JI2B( itsMacroMgr != NULL );
}

/******************************************************************************
 GetToolBar

 ******************************************************************************/

inline JXToolBar*
CBTextDocument::GetToolBar()
	const
{
	return itsToolBar;
}

/******************************************************************************
 GetCommandMenu (protected)

 ******************************************************************************/

inline CBCommandMenu*
CBTextDocument::GetCommandMenu()
{
	return itsCmdMenu;
}

#endif
