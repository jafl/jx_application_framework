/******************************************************************************
 GFGPrefsManager.cpp

	<Description>

	BASE CLASS = public JXPrefsManager

	Copyright (C) 2002 by New Planet Software.

 *****************************************************************************/

#include "GFGPrefsManager.h"
#include "GFGPrefsDialog.h"
#include "gfgGlobals.h"
#include <JXChooseSaveFile.h>
#include <time.h>
#include <jAssert.h>

const JFileVersion kCurrentPrefsFileVersion = 0;

/******************************************************************************
 Constructor

 *****************************************************************************/

GFGPrefsManager::GFGPrefsManager
	(
	JBoolean* isNew
	)
	:
	JXPrefsManager(kCurrentPrefsFileVersion, kJTrue)
{
	itsDialog = nullptr;
	*isNew    = JPrefsManager::UpgradeData();

	JXChooseSaveFile* csf = JXGetChooseSaveFile();
	csf->SetPrefInfo(this, kGFGgCSFSetupID);
	csf->JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GFGPrefsManager::~GFGPrefsManager()
{
	SaveAllBeforeDestruct();
}

/******************************************************************************
 SaveAllBeforeDestruct (virtual protected)

 ******************************************************************************/

void
GFGPrefsManager::SaveAllBeforeDestruct()
{
	SetData(kGFGProgramVersionID, GFGGetVersionNumberStr());

	JXPrefsManager::SaveAllBeforeDestruct();
}

/******************************************************************************
 UpgradeData (virtual protected)

 ******************************************************************************/

void
GFGPrefsManager::UpgradeData
	(
	const JBoolean		isNew,
	const JFileVersion	currentVersion
	)
{
	if (isNew)
		{
		SetData(kGFGProgramVersionID, GFGGetVersionNumberStr());
		}
}

/******************************************************************************
 GetPrevVersionStr

 ******************************************************************************/

JString
GFGPrefsManager::GetPrevVersionStr()
	const
{
	std::string data;
	const JBoolean ok = GetData(kGFGProgramVersionID, &data);
	assert( ok );
	return JString(data);
}

/******************************************************************************
 EditPrefs

 ******************************************************************************/

void
GFGPrefsManager::EditPrefs()
{
	assert( itsDialog == nullptr );

	// replace with whatever is appropriate
	JString data;

	itsDialog = 
		jnew GFGPrefsDialog(JXGetApplication(), 
			GetHeaderComment(),
			GetSourceComment(),
			GetConstructorComment(),
			GetDestructorComment(),
			GetFunctionComment());
	assert( itsDialog != nullptr );
	ListenTo(itsDialog);
	itsDialog->BeginDialog();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GFGPrefsManager::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			// replace with whatever is appropriate
			JString header;
			JString source;
			JString constructor;
			JString destructor;
			JString function;

			itsDialog->GetValues(&header, &source,
					  &constructor, &destructor,
					  &function);

			SetHeaderComment(header);
			SetSourceComment(source);
			SetConstructorComment(constructor);
			SetDestructorComment(destructor);
			SetFunctionComment(function);

			// store data somehow
			}
		itsDialog = nullptr;
		}

	else
		{
		JXPrefsManager::Receive(sender, message);
		}
}

/******************************************************************************
 GetHeaderComment (public)

 ******************************************************************************/

JString
GFGPrefsManager::GetHeaderComment
	(
	const JCharacter* classname
	)
	const
{
	JString comment;
	std::string data;
	if (GetData(kGFGHeaderCommentID, &data))
		{
		std::istringstream is(data);
		is >> comment;
		}
	else
		{
		comment	= JGetString("CLASS_HEADER_COMMENT");
		}
		
	if (classname != nullptr)
		{
		JString copyright	= GetCopyright(kJTrue);
		const JCharacter* map[] =
			{
			"class", classname,
			"copyright", copyright
			};
		JGetStringManager()->Replace(&comment, map, sizeof(map));
		}

	return comment;
}

/******************************************************************************
 SetHeaderComment (public)

 ******************************************************************************/

void
GFGPrefsManager::SetHeaderComment
	(
	const JCharacter* comment
	)
{
	std::ostringstream os;
	JString str(comment);
	os << str;
	SetData(kGFGHeaderCommentID, os);
}

/******************************************************************************
 GetAuthor (public)

 ******************************************************************************/

JString
GFGPrefsManager::GetAuthor()
	const
{
	std::string data;
	if (GetData(kGFGAuthorID, &data))
		{
		std::istringstream is(data);
		JString author;
		is >> author;
		return author;
		}
	else
		{
		return "";
		}
}

/******************************************************************************
 SetAuthor (public)

 ******************************************************************************/

void
GFGPrefsManager::SetAuthor
	(
	const JCharacter* author
	)
{
	std::ostringstream os;
	JString str(author);
	os << str;
	SetData(kGFGAuthorID, os);
}

/******************************************************************************
 GetYear (public)

 ******************************************************************************/

JString
GFGPrefsManager::GetYear()
	const
{
	std::string data;
	if (GetData(kGFGYearID, &data))
		{
		std::istringstream is(data);
		JString year;
		is >> year;
		return year;
		}
	else
		{
		time_t now;
		time(&now);

		struct tm* local_time	= localtime(&now);

		return JString(local_time->tm_year + 1900, JString::kBase10);
		}
}

/******************************************************************************
 SetYear (public)

 ******************************************************************************/

void
GFGPrefsManager::SetYear
	(
	const JCharacter* year
	)
{
	std::ostringstream os;
	JString str(year);
	os << str;
	SetData(kGFGYearID, os);
}


/******************************************************************************
 GetCopyright (public)

 ******************************************************************************/

JString
GFGPrefsManager::GetCopyright
	(
	const JBoolean replaceVars
	)
	const
{
	JString copyright;
	std::string data;
	if (GetData(kGFGCopyrightID, &data))
		{
		std::istringstream is(data);
		is >> copyright;
		}
	else
		{
		copyright	= JGetString("CLASS_COPYRIGHT");
		}
		
	if (replaceVars)
		{
		JString author	= GetAuthor();
		JString year	= GetYear();
		const JCharacter* map[] =
			{
			"year", year,
			"author", author
			};
		JGetStringManager()->Replace(&copyright, map, sizeof(map));
		}

	return copyright;
}

/******************************************************************************
 SetCopyright (public)

 ******************************************************************************/

void
GFGPrefsManager::SetCopyright
	(
	const JCharacter* copyright
	)
{
	std::ostringstream os;
	JString str(copyright);
	os << str;
	SetData(kGFGCopyrightID, os);
}

/******************************************************************************
 GetSourceComment (public)

 ******************************************************************************/

JString
GFGPrefsManager::GetSourceComment
	(
	const JCharacter* classname,
	const JCharacter* base
	)
	const
{
	JString comment;
	std::string data;
	if (GetData(kGFGSourceCommentID, &data))
		{
		std::istringstream is(data);
		is >> comment;
		}
	else
		{
		comment	= JGetString("CLASS_SOURCE_COMMENT");
		}
		
	if (classname != nullptr)
		{
		JString copyright	= GetCopyright(kJTrue);
		const JCharacter* map[] =
			{
			"class", classname,
			"base", base,
			"copyright", copyright
			};
		JGetStringManager()->Replace(&comment, map, sizeof(map));
		}

	return comment;
}

/******************************************************************************
 SetSourceComment (public)

 ******************************************************************************/

void
GFGPrefsManager::SetSourceComment
	(
	const JCharacter* comment
	)
{
	std::ostringstream os;
	JString str(comment);
	os << str;
	SetData(kGFGSourceCommentID, os);
}

/******************************************************************************
 GetConstructorComment (public)

 ******************************************************************************/

JString
GFGPrefsManager::GetConstructorComment()
	const
{
	JString comment;
	std::string data;
	if (GetData(kGFGConstructorCommentID, &data))
		{
		std::istringstream is(data);
		is >> comment;
		}
	else
		{
		comment	= JGetString("CLASS_CONSTRUCTOR_COMMENT");
		}

	return comment;
}

/******************************************************************************
 SetConstructorComment

 ******************************************************************************/

void
GFGPrefsManager::SetConstructorComment
	(
	const JCharacter* comment
	)
{
	std::ostringstream os;
	JString str(comment);
	os << str;
	SetData(kGFGConstructorCommentID, os);
}

/******************************************************************************
 GetDestructorComment (public)

 ******************************************************************************/

JString
GFGPrefsManager::GetDestructorComment()
	const
{
	JString comment;
	std::string data;
	if (GetData(kGFGDestructorCommentID, &data))
		{
		std::istringstream is(data);
		is >> comment;
		}
	else
		{
		comment	= JGetString("CLASS_DESTRUCTOR_COMMENT");
		}
		
	return comment;
}

/******************************************************************************
 SetDestructorComment

 ******************************************************************************/

void
GFGPrefsManager::SetDestructorComment
	(
	const JCharacter* comment
	)
{
	std::ostringstream os;
	JString str(comment);
	os << str;
	SetData(kGFGDestructorCommentID, os);
}

/******************************************************************************
 GetFunctionComment (public)

 ******************************************************************************/

JString
GFGPrefsManager::GetFunctionComment
	(
	const JCharacter* fnname,
	const JCharacter* access
	)
	const
{
	JString comment;
	std::string data;
	if (GetData(kGFGFunctionCommentID, &data))
		{
		std::istringstream is(data);
		is >> comment;
		}
	else
		{
		comment	= JGetString("CLASS_FUNCTION_COMMENT");
		}

	if (fnname != nullptr)
		{
		const JCharacter* map[] =
			{
			"function", fnname,
			"access", access
			};
		JGetStringManager()->Replace(&comment, map, sizeof(map));
		}
			
	return comment;
}

/******************************************************************************
 SetFunctionComment

 ******************************************************************************/

void
GFGPrefsManager::SetFunctionComment
	(
	const JCharacter* comment
	)
{
	std::ostringstream os;
	JString str(comment);
	os << str;
	SetData(kGFGFunctionCommentID, os);
}
