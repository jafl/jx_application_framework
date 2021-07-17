/******************************************************************************
 CBTextDocument.h

	Interface for the CBTextDocument class

	Copyright © 1996-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBTextDocument
#define _H_CBTextDocument

#include <JXFileDocument.h>
#include <JPrefObject.h>
#include "CBTextFileType.h"
#include <JXWidget.h>		// need defn of H/VSizing
#include <JStyledText.h>	// need defn of PlainTextFormat, CRMRuleList

class JSTStyler;
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
											const JString& fileName,
											JXMenuBar* menuBar,
											CBTELineIndexInput* lineInput,
											CBTEColIndexInput* colInput,
											JXScrollbarSet* scrollbarSet);

class CBTextDocument : public JXFileDocument, public JPrefObject
{
public:

	CBTextDocument(const CBTextFileType type = kCBUnknownFT,
				   const JUtf8Byte* helpSectionName = "CBEditorHelp",
				   const bool setWMClass = true,
				   CBTextEditorCtorFn* teCtorFn = ConstructTextEditor);
	CBTextDocument(const JString& fileName,
				   const CBTextFileType type = kCBUnknownFT,
				   const bool tmpl = false);
	CBTextDocument(std::istream& input, const JFileVersion vers, bool* keep);

	virtual ~CBTextDocument();

	virtual void		Activate() override;
	virtual bool	GetMenuIcon(const JXImage** icon) const override;

	void	GoToLine(const JIndex lineIndex) const;
	void	SelectLines(const JIndexRange& range) const;
	void	EditPrefs();
	void	HandleActionButton();

	void	WriteForProject(std::ostream& output) const;

	bool	GetWindowSize(JPoint* desktopLoc,
							  JCoordinate* width, JCoordinate* height) const;
	void		SaveWindowSize() const;

	bool	WillOpenComplFileOnTop() const;
	void		ShouldOpenComplFileOnTop(const bool doIt);

	CBTextEditor*	GetTextEditor() const;
	JXMenuBar*		GetMenuBar() const;
	JXToolBar*		GetToolBar() const;
	JRect			GetFileDisplayInfo(JXWidget::HSizingOption* hSizing,
									   JXWidget::VSizingOption* vSizing) const;
	void			SetFileDisplayVisible(const bool show);

	CBTextFileType	GetFileType() const;
	bool		GetStyler(CBStylerBase** styler) const;
	bool		GetStringCompleter(CBStringCompleter** completer) const;
	bool		GetCharActionManager(CBCharActionManager** mgr) const;
	bool		GetMacroManager(CBMacroManager** mgr) const;

	void	OverrideTabWidth(const JSize tabWidth);
	void	OverrideAutoIndent(const bool autoIndent);
	void	OverrideTabInsertsSpaces(const bool insertSpaces);
	void	OverrideShowWhitespace(const bool showWhitespace);
	void	OverrideBreakCROnly(const bool breakCROnly);
	void	OverrideReadOnly(const bool readOnly);

	static void	ReadStaticGlobalPrefs(std::istream& input, const JFileVersion vers);
	static void	WriteStaticGlobalPrefs(std::ostream& output);

	// called by CBDocumentManager

	void	UpdateFileType(const bool init = false);
	void	StylerChanged(JSTStyler* styler);

	void	RefreshVCSStatus();

	static bool	OpenAsBinaryFile(const JString& fileName);

	// called by CBTextEditor

	virtual void	ConvertSelectionToFullPath(JString* fileName) const;

protected:

	JXTextMenu*		InsertTextMenu(const JString& title);
	CBCommandMenu*	GetCommandMenu();

	void			ReadFile(const JString& fileName, const bool firstTime);
	virtual void	WriteTextFile(std::ostream& output, const bool safetySave) const override;
	virtual void	DiscardChanges() override;

	virtual void	HandleFileModifiedByOthers(const bool modTimeChanged,
											   const bool permsChanged) override;

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

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
	bool			itsOpenOverComplementFlag;	// true if open window over complement file
	JXTextMenu*			itsFileMenu;
	JXTextMenu*			itsFileFormatMenu;
	JXTextMenu*			itsDiffMenu;
	CBCommandMenu*		itsCmdMenu;
	JXTextMenu*			itsWindowMenu;
	JXTextMenu*			itsPrefsMenu;
	JXTextMenu*			itsPrefsStylesMenu;
	JXTextMenu*			itsHelpMenu;

	CBTextFileType				itsFileType;
	CBCharActionManager*		itsActionMgr;		// not owned; can be nullptr
	CBMacroManager*				itsMacroMgr;		// not owned; can be nullptr
	JStyledText::CRMRuleList*	itsCRMRuleList;		// not owned; can be nullptr

	const JUtf8Byte*	itsHelpSectionName;
	bool			itsUpdateFileTypeFlag;		// false while constructing
	bool			itsOverrideFlag[ kSettingCount ];

	JStyledText::PlainTextFormat	itsFileFormat;

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
	void	CBTextDocumentX2(const bool setWindowSize);
	void	BuildWindow(const bool setWMClass,
						CBTextEditorCtorFn* teCtorFn);
	void	DisplayFileName(const JString& name);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex item);

	bool	OpenSomething();

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

	bool	ReadFromProject(std::istream& input, const JFileVersion vers);

	void	UpdateReadOnlyDisplay();

	static CBTextEditor*	ConstructTextEditor(CBTextDocument* document,
												const JString& fileName,
												JXMenuBar* menuBar,
												CBTELineIndexInput* lineInput,
												CBTEColIndexInput* colInput,
												JXScrollbarSet* scrollbarSet);

	// not allowed

	CBTextDocument(const CBTextDocument& source);
	const CBTextDocument& operator=(const CBTextDocument& source);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kSaved;

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

inline bool
CBTextDocument::WillOpenComplFileOnTop()
	const
{
	return itsOpenOverComplementFlag;
}

inline void
CBTextDocument::ShouldOpenComplFileOnTop
	(
	const bool doIt
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

inline bool
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

inline bool
CBTextDocument::GetCharActionManager
	(
	CBCharActionManager** mgr
	)
	const
{
	*mgr = itsActionMgr;
	return itsActionMgr != nullptr;
}

/******************************************************************************
 GetMacroManager

	Returns the CBMacroManager to be used for this document, if any.

 ******************************************************************************/

inline bool
CBTextDocument::GetMacroManager
	(
	CBMacroManager** mgr
	)
	const
{
	*mgr = itsMacroMgr;
	return itsMacroMgr != nullptr;
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
