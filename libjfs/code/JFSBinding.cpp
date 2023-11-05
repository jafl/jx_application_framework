/******************************************************************************
 JFSBinding.cpp

	BASE CLASS = none

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "JFSBinding.h"
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JSubstitute.h>
#include <jx-af/jcore/JDirInfo.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kOrigDefaultMarker  = "<default>";
static const JUtf8Byte* kNameRegexMarker    = "*";
static const JUtf8Byte* kContentRegexMarker = "^";

/******************************************************************************
 Constructor

 ******************************************************************************/

JFSBinding::JFSBinding
	(
	const JString&		pattern,
	const JString&		cmd,
	const CommandType	type,
	const bool		singleFile,
	const bool		isSystem
	)
	:
	itsPattern(pattern),
	itsCmd(cmd),
	itsCmdType(type),
	itsSingleFileFlag(singleFile),
	itsIsSystemFlag(isSystem),
	itsNameRegex(nullptr),
	itsContentRegex(nullptr)
{
	UpdateRegex();
}

JFSBinding::JFSBinding
	(
	std::istream&		input,
	const JFileVersion	vers,
	const bool		isSystem,
	bool*			isDefault,
	bool*			del
	)
	:
	itsIsSystemFlag(isSystem),
	itsNameRegex(nullptr),
	itsContentRegex(nullptr)
{
	input >> itsPattern >> itsCmd;

	if (vers < 2)
	{
		ConvertCommand(&itsCmd);
		JIgnoreLine(input);		// alternate cmd

		*isDefault = itsPattern == kOrigDefaultMarker;
		if (*isDefault)
		{
			itsPattern.Clear();
		}

		itsCmdType        = kRunPlain;
		itsSingleFileFlag = itsCmd.Contains("$");
	}
	else
	{
		*isDefault = itsPattern.IsEmpty();

		input >> itsCmdType >> JBoolFromString(itsSingleFileFlag);
	}

	*del = itsCmd.IsEmpty();

	if (!(*del))
	{
		UpdateRegex();
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JFSBinding::~JFSBinding()
{
	jdelete itsNameRegex;
	jdelete itsContentRegex;
}

/******************************************************************************
 Match

 ******************************************************************************/

bool
JFSBinding::Match
	(
	const JString& fileName,
	const JString& content
	)
	const
{
	if (itsCmd.IsEmpty())
	{
		return false;
	}
	else if (itsContentRegex != nullptr)
	{
		return (content.StartsWith(itsLiteralPrefix) &&
				itsContentRegex->Match(content));
	}
	else if (itsNameRegex != nullptr)
	{
		return itsNameRegex->Match(fileName);
	}
	else
	{
		return (!itsPattern.IsEmpty() &&
				fileName.EndsWith(itsPattern, JString::kIgnoreCase));
	}
}

/******************************************************************************
 UpdateRegex (private)

 ******************************************************************************/

void
JFSBinding::UpdateRegex()
{
	if (itsPattern.StartsWith(kContentRegexMarker))
	{
		jdelete itsNameRegex;
		itsNameRegex = nullptr;

		if (itsContentRegex == nullptr)
		{
			itsContentRegex = jnew JRegex;
			itsContentRegex->SetSingleLine(true);
		}

		if (itsContentRegex->SetPattern(itsPattern).OK())
		{
			CalcLiteralPrefix();
		}
		else
		{
			jdelete itsContentRegex;
			itsContentRegex = nullptr;
		}
	}
	else if (itsPattern.Contains(kNameRegexMarker))
	{
		jdelete itsContentRegex;
		itsContentRegex = nullptr;

		if (itsNameRegex == nullptr)
		{
			itsNameRegex = jnew JRegex;
		}

		JString s;
		const bool ok = JDirInfo::BuildRegexFromWildcardFilter(itsPattern, &s);
		assert( ok );
		if (!itsNameRegex->SetPattern(s).OK())
		{
			jdelete itsNameRegex;
			itsNameRegex = nullptr;
		}
	}
	else
	{
		jdelete itsNameRegex;
		itsNameRegex = nullptr;

		jdelete itsContentRegex;
		itsContentRegex = nullptr;
	}
}

/******************************************************************************
 WillBeRegex (static)

 ******************************************************************************/

bool
JFSBinding::WillBeRegex
	(
	const JString& pattern
	)
{
	return pattern.StartsWith(kContentRegexMarker);
}

/******************************************************************************
 CalcLiteralPrefix (private)

 ******************************************************************************/

void
JFSBinding::CalcLiteralPrefix()
{
	const JUtf8Byte* s = itsPattern.GetBytes();

	JIndex i = 1;	// skip leading ^
	while (s[i] != '\0' && s[i] != '\\' &&
		   !JRegex::NeedsBackslashToBeLiteral(s[i]))
	{
		i++;
	}

	if (s[i] == '?')
	{
		i--;
	}

	itsLiteralPrefix.Set(itsPattern, JCharacterRange(2, i));
}

/******************************************************************************
 ConvertCommand (static)

 ******************************************************************************/

void
JFSBinding::ConvertCommand
	(
	JString* cmd
	)
{
	JStringIterator iter(cmd);
	while (iter.Next("\"$f\""))
	{
		iter.ReplaceLastMatch("$q");
	}

	iter.MoveTo(JStringIterator::kStartAtBeginning, 0);
	while (iter.Next("$f"))
	{
		iter.ReplaceLastMatch("$u");
	}
}

/******************************************************************************
 GetCommandType (static)

 ******************************************************************************/

JFSBinding::CommandType
JFSBinding::GetCommandType
	(
	const bool shell,
	const bool window
	)
{
	if (window)
	{
		return JFSBinding::kRunInWindow;
	}
	else if (shell)
	{
		return JFSBinding::kRunInShell;
	}
	else
	{
		return JFSBinding::kRunPlain;
	}
}

/******************************************************************************
 ComparePatterns (static)

 ******************************************************************************/

std::weak_ordering
JFSBinding::ComparePatterns
	(
	JFSBinding* const & n1,
	JFSBinding* const & n2
	)
{
	return JCompareStringsCaseInsensitive(const_cast<JString*>(&(n1->GetPattern())),
										  const_cast<JString*>(&(n2->GetPattern())));
}

/******************************************************************************
 Global functions

 ******************************************************************************/

/******************************************************************************
 Stream operators for JFSBinding

 ******************************************************************************/

std::ostream&
operator<<
	(
	std::ostream&		output,
	const JFSBinding&	binding
	)
{
	JFSBinding::CommandType type;
	bool singleFile;

	output << binding.GetPattern();
	output << ' ' << binding.GetCommand(&type, &singleFile);
	output << ' ' << type;
	output << ' ' << JBoolToString(singleFile);
	return output;
}

/******************************************************************************
 Stream operators for JFSBinding::CommandType

 ******************************************************************************/

const JUtf8Byte kPlainMarker  = 'p';
const JUtf8Byte kShellMarker  = 's';
const JUtf8Byte kWindowMarker = 'w';

std::istream&
operator>>
	(
	std::istream&					input,
	JFSBinding::CommandType&	type
	)
{
	input >> std::ws;

	JUtf8Byte c;
	input.get(c);

	if (c == kPlainMarker)
	{
		type = JFSBinding::kRunPlain;
	}
	else if (c == kShellMarker)
	{
		type = JFSBinding::kRunInShell;
	}
	else if (c == kWindowMarker)
	{
		type = JFSBinding::kRunInWindow;
	}
	else
	{
		input.putback(c);
		JSetState(input, std::ios::failbit);
	}

	return input;
}

std::ostream&
operator<<
	(
	std::ostream&					output,
	const JFSBinding::CommandType	type
	)
{
	if (type == JFSBinding::kRunPlain)
	{
		output << kPlainMarker;
	}
	else if (type == JFSBinding::kRunInShell)
	{
		output << kShellMarker;
	}
	else if (type == JFSBinding::kRunInWindow)
	{
		output << kWindowMarker;
	}
	else
	{
		assert( 0 );	// no other possible values for JFSBinding::CommandType
	}

	return output;
}
