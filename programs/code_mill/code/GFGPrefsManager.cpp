/******************************************************************************
 GFGPrefsManager.cpp

	<Description>

	BASE CLASS = public JXPrefsManager

	Copyright (C) 2002 by Glenn W. Bach.

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
	bool* isNew
	)
	:
	JXPrefsManager(kCurrentPrefsFileVersion, true)
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
	const bool		isNew,
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
	const bool ok = GetData(kGFGProgramVersionID, &data);
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
		const auto* info =
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
 GetHeaderComment

 ******************************************************************************/

JString
GFGPrefsManager::GetHeaderComment
	(
	const JString& classname
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
		
	if (!classname.IsEmpty())
		{
		JString copyright = GetCopyright(true);
		const JUtf8Byte* map[] =
			{
			"class",     classname.GetBytes(),
			"copyright", copyright.GetBytes()
			};
		JGetStringManager()->Replace(&comment, map, sizeof(map));
		}

	return comment;
}

/******************************************************************************
 SetHeaderComment

 ******************************************************************************/

void
GFGPrefsManager::SetHeaderComment
	(
	const JString& comment
	)
{
	std::ostringstream os;
	os << comment;
	SetData(kGFGHeaderCommentID, os);
}

/******************************************************************************
 GetAuthor

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
		return JString::empty;
		}
}

/******************************************************************************
 SetAuthor

 ******************************************************************************/

void
GFGPrefsManager::SetAuthor
	(
	const JString& author
	)
{
	std::ostringstream os;
	os << author;
	SetData(kGFGAuthorID, os);
}

/******************************************************************************
 GetYear

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

		return JString((JUInt64) local_time->tm_year + 1900);
		}
}

/******************************************************************************
 SetYear

 ******************************************************************************/

void
GFGPrefsManager::SetYear
	(
	const JString& year
	)
{
	std::ostringstream os;
	os << year;
	SetData(kGFGYearID, os);
}


/******************************************************************************
 GetCopyright

 ******************************************************************************/

JString
GFGPrefsManager::GetCopyright
	(
	const bool replaceVars
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
		JString author = GetAuthor();
		JString year   = GetYear();
		const JUtf8Byte* map[] =
			{
			"year",   year.GetBytes(),
			"author", author.GetBytes()
			};
		JGetStringManager()->Replace(&copyright, map, sizeof(map));
		}

	return copyright;
}

/******************************************************************************
 SetCopyright

 ******************************************************************************/

void
GFGPrefsManager::SetCopyright
	(
	const JString& copyright
	)
{
	std::ostringstream os;
	os << copyright;
	SetData(kGFGCopyrightID, os);
}

/******************************************************************************
 GetSourceComment

 ******************************************************************************/

JString
GFGPrefsManager::GetSourceComment
	(
	const JString& classname,
	const JString& base
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
		
	if (!classname.IsEmpty())
		{
		JString copyright = GetCopyright(true);
		const JUtf8Byte* map[] =
			{
			"class",     classname.GetBytes(),
			"base",      base.GetBytes(),
			"copyright", copyright.GetBytes()
			};
		JGetStringManager()->Replace(&comment, map, sizeof(map));
		}

	return comment;
}

/******************************************************************************
 SetSourceComment

 ******************************************************************************/

void
GFGPrefsManager::SetSourceComment
	(
	const JString& comment
	)
{
	std::ostringstream os;
	os << comment;
	SetData(kGFGSourceCommentID, os);
}

/******************************************************************************
 GetConstructorComment

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
	const JString& comment
	)
{
	std::ostringstream os;
	os << comment;
	SetData(kGFGConstructorCommentID, os);
}

/******************************************************************************
 GetDestructorComment

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
	const JString& comment
	)
{
	std::ostringstream os;
	os << comment;
	SetData(kGFGDestructorCommentID, os);
}

/******************************************************************************
 GetFunctionComment

 ******************************************************************************/

JString
GFGPrefsManager::GetFunctionComment
	(
	const JString& fnName,
	const JString& access
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

	if (!fnName.IsEmpty())
		{
		const JUtf8Byte* map[] =
			{
			"function", fnName.GetBytes(),
			"access",   access.GetBytes()
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
	const JString& comment
	)
{
	std::ostringstream os;
	os << comment;
	SetData(kGFGFunctionCommentID, os);
}
