/******************************************************************************
 JXHelpManager.h

	Copyright (C) 1997-2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXHelpManager
#define _H_JXHelpManager

#include <JXDirector.h>
#include <JXSharedPrefObject.h>

class JString;
class JXWindow;
class JXHelpDirector;
class JXPSPrinter;

class JXHelpManager : public JXDirector, public JXSharedPrefObject
{
public:

	JXHelpManager();

	virtual ~JXHelpManager();

	JBoolean	HasTOC() const;
	void		ShowTOC();
	void		SetTOCSectionName(const JUtf8Byte* name);

	void	RegisterSection(const JUtf8Byte* name);
	void	ShowSection(const JUtf8Byte* name, JXHelpDirector* helpDir = NULL);
	void	ShowURL(const JString& url, JXHelpDirector* helpDir = NULL);
	void	CloseAll();

	void	SearchAllSections();

	void	SaveWindowPrefs(JXWindow* window);

	static JBoolean	IsLocalURL(const JString& url);

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output, const JFileVersion vers) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	DirectorClosed(JXDirector* theDirector);

private:

	struct SectionInfo
	{
		const JUtf8Byte*	name;
		JXHelpDirector*		dir;

		SectionInfo()
			:
			name(NULL), dir(NULL)
		{ };

		SectionInfo(const JUtf8Byte* n)
			:
			name(n), dir(NULL)
		{ };
	};

private:

	JArray<SectionInfo>*	itsSections;
	const JUtf8Byte*		itsTOCSectionName;	// NULL if not registered
	JXHelpDirector*			itsComposeHelpDir;	// NULL if not visible

	JString 				itsDefWindowGeom;
	JXPSPrinter*			itsPrinter;

private:

	JXHelpDirector*	CreateHelpDirector(const JString& text);

	static JListT::CompareResult
		CompareSections(const SectionInfo& s1, const SectionInfo& s2);

	// not allowed

	JXHelpManager(const JXHelpManager& source);
	const JXHelpManager& operator=(const JXHelpManager& source);
};


/******************************************************************************
 Table of Contents

 ******************************************************************************/

inline JBoolean
JXHelpManager::HasTOC()
	const
{
	return JI2B(itsTOCSectionName != NULL);
}

inline void
JXHelpManager::SetTOCSectionName
	(
	const JUtf8Byte* name
	)
{
	itsTOCSectionName = name;
}

#endif
