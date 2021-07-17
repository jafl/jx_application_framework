/******************************************************************************
 CMSourceDirector.h

	Interface for the CMSourceDirector class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_CMSourceDirector
#define _H_CMSourceDirector

#include <JXWindowDirector.h>
#include "CMLocation.h"

class JXMenuBar;
class JXTextMenu;
class JXFileNameDisplay;
class JXToolBar;
class CMLink;
class CMCommandDirector;
class CMLineIndexTable;
class CMSourceText;
class CMDisplaySourceForMain;
class CMGetAssembly;
class CBFunctionMenu;

class CMSourceDirector : public JXWindowDirector
{
public:

	enum Type
	{
		kSourceType,
		kMainSourceType,
		kMainAsmType,
		kAsmType
	};

public:

	CMSourceDirector(CMCommandDirector* commandDir, const Type type);

	static CMSourceDirector*	Create(CMCommandDirector* commandDir,
									   const JString& fileOrFn,
									   const Type type);

	void	CreateWindowsMenu();

	virtual ~CMSourceDirector();

	CMCommandDirector*	GetCommandDirector();

	bool	IsMainSourceWindow() const;
	bool	GetFileName(const JString** fileName) const;
	JIndex		GetCurrentExecLine() const;

	bool	GetFunctionName(const JString** fnName) const;

	void	DisplayFile(const JString& fileName, const JIndex lineNumber = 0,
						const bool markLine = true);
	void	DisplayDisassembly(const CMLocation& loc);
	void	DisplayLine(const JSize lineNumber, const bool markLine = true);
	void	ClearDisplay();

	Type					GetType() const;
	virtual const JString&	GetName() const override;
	virtual bool		GetMenuIcon(const JXImage** icon) const override;

	// called by GDBGetAssembly

	const CMLocation&	GetDisassemblyLocation() const;
	void				DisplayDisassembly(JPtrArray<JString>* addrList,
										   const JString& instText);

protected:

	CMSourceDirector(CMCommandDirector* commandDir, const JString& fileOrFn,
					 const Type type);

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	CMLink*				itsLink;
	CMCommandDirector*	itsCommandDir;
	const Type			itsType;
	CMLineIndexTable*	itsTable;
	CMSourceText*		itsText;
	JString				itsCurrentFile;			// can be empty
	JString				itsCurrentFn;			// can be empty
	CMLocation			itsAsmLocation;			// used while waiting for CMGetAssembly
	JString				itsWindowTitlePrefix;	// only used by main src window

	JXTextMenu*		itsFileMenu;
	JXTextMenu*		itsDebugMenu;
	CBFunctionMenu*	itsFnMenu;
	JXTextMenu*		itsPrefsMenu;
	JXTextMenu*		itsHelpMenu;

	CMDisplaySourceForMain*	itsSrcMainCmd;
	CMGetAssembly*			itsGetAssemblyCmd;	// can be nullptr

// begin JXLayout

	JXMenuBar*         itsMenuBar;
	JXToolBar*         itsToolBar;
	JXFileNameDisplay* itsFileDisplay;

// end JXLayout

private:

	void	CMSourceViewDirectorX(CMCommandDirector* commandDir);
	void	BuildWindow();
	void	UpdateFileType();
	void	UpdateWindowTitle(const JString& binaryName);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdatePrefsMenu();
	void	HandlePrefsMenu(const JIndex index);

	void	HandleHelpMenu(const JIndex index);

	// not allowed

	CMSourceDirector(const CMSourceDirector& source);
	const CMSourceDirector& operator=(const CMSourceDirector& source);
};


/******************************************************************************
 GetCommandDirector

 ******************************************************************************/

inline CMCommandDirector*
CMSourceDirector::GetCommandDirector()
{
	return itsCommandDir;
}

/******************************************************************************
 GetType

 ******************************************************************************/

inline CMSourceDirector::Type
CMSourceDirector::GetType()
	const
{
	return itsType;
}

/******************************************************************************
 IsMainSourceWindow

 ******************************************************************************/

inline bool
CMSourceDirector::IsMainSourceWindow()
	const
{
	return itsType != kSourceType && itsType != kAsmType;
}

/******************************************************************************
 GetFileName

 ******************************************************************************/

inline bool
CMSourceDirector::GetFileName
	(
	const JString** fileName
	)
	const
{
	*fileName = &itsCurrentFile;
	return !itsCurrentFile.IsEmpty();
}

/******************************************************************************
 GetFunctionName

 ******************************************************************************/

inline bool
CMSourceDirector::GetFunctionName
	(
	const JString** fnName
	)
	const
{
	*fnName = &itsCurrentFn;
	return !itsCurrentFn.IsEmpty();
}

/******************************************************************************
 GetDisassemblyLocation

 ******************************************************************************/

inline const CMLocation&
CMSourceDirector::GetDisassemblyLocation()
	const
{
	return itsAsmLocation;
}

#endif
