/******************************************************************************
 JWebBrowser.h

	Copyright (C) 2000-2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_JWebBrowser
#define _H_JWebBrowser

#include "JPtrArray-JString.h"

class JWebBrowser
{
public:

	JWebBrowser();

	virtual ~JWebBrowser();

	void	ShowURL(const JString& url);
	void	ShowFileContent(const JString& fileName);
	void	ShowFileLocation(const JString& fileName);
	void	ShowFileLocations(const JPtrArray<JString>& fileList);
	void	ComposeMail(const JString& address);

	const JString&	GetShowURLCmd() const;
	void			SetShowURLCmd(const JString& cmd);

	const JString&	GetShowFileContentCmd() const;
	void			SetShowFileContentCmd(const JString& cmd);

	const JString&	GetShowFileLocationCmd() const;
	void			SetShowFileLocationCmd(const JString& cmd);

	const JString&	GetComposeMailCmd() const;
	void			SetComposeMailCmd(const JString& cmd);

	void	ReadConfig(std::istream& input);
	void	WriteConfig(std::ostream& output, const JFileVersion vers) const;

	static JFileVersion	GetCurrentConfigVersion();

	// upgrading from previous versions -- public because used by other classes

	static void	ConvertVarNames(JString* s, const JUtf8Byte* varNameList);

protected:

	virtual void	SaveCommands();

private:

	JString	itsShowURLCmd;
	JString	itsShowFileContentCmd;
	JString	itsShowFileLocationCmd;
	JString	itsComposeMailCmd;

private:

	void	Exec(const JString& cmd,
				 const JUtf8Byte* varName, const JString& value) const;

	// not allowed

	JWebBrowser(const JWebBrowser&) = delete;
	JWebBrowser& operator=(const JWebBrowser&) = delete;
};


/******************************************************************************
 Get outsourced commands

 ******************************************************************************/

inline const JString&
JWebBrowser::GetShowURLCmd()
	const
{
	return itsShowURLCmd;
}

inline const JString&
JWebBrowser::GetShowFileContentCmd()
	const
{
	return itsShowFileContentCmd;
}

inline const JString&
JWebBrowser::GetShowFileLocationCmd()
	const
{
	return itsShowFileLocationCmd;
}

inline const JString&
JWebBrowser::GetComposeMailCmd()
	const
{
	return itsComposeMailCmd;
}

/******************************************************************************
 Set outsourced commands

 ******************************************************************************/

inline void
JWebBrowser::SetShowURLCmd
	(
	const JString& cmd
	)
{
	itsShowURLCmd = cmd;
	SaveCommands();
}

inline void
JWebBrowser::SetShowFileContentCmd
	(
	const JString& cmd
	)
{
	itsShowFileContentCmd = cmd;
	SaveCommands();
}

inline void
JWebBrowser::SetShowFileLocationCmd
	(
	const JString& cmd
	)
{
	itsShowFileLocationCmd = cmd;
	SaveCommands();
}

inline void
JWebBrowser::SetComposeMailCmd
	(
	const JString& cmd
	)
{
	itsComposeMailCmd = cmd;
	SaveCommands();
}

#endif
