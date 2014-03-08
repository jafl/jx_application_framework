/******************************************************************************
 JFSBinding.cpp

	BASE CLASS = none

	Copyright � 1997 by Glenn Bach.  All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include "JFSBinding.h"
#include <JRegex.h>
#include <JSubstitute.h>
#include <JDirInfo.h>
#include <jStreamUtil.h>
#include <jAssert.h>

static const JCharacter* kOrigDefaultMarker  = "<default>";
static const JCharacter* kNameRegexMarker    = "*";
static const JCharacter* kContentRegexMarker = "^";

/******************************************************************************
 Constructor

 ******************************************************************************/

JFSBinding::JFSBinding
	(
	const JCharacter*	pattern,
	const JCharacter*	cmd,
	const CommandType	type,
	const JBoolean		singleFile,
	const JBoolean		isSystem
	)
	:
	itsPattern(pattern),
	itsCmd(cmd),
	itsCmdType(type),
	itsSingleFileFlag(singleFile),
	itsIsSystemFlag(isSystem),
	itsNameRegex(NULL),
	itsContentRegex(NULL)
{
	UpdateRegex();
}

JFSBinding::JFSBinding
	(
	istream&			input,
	const JFileVersion	vers,
	const JBoolean		isSystem,
	JBoolean*			isDefault,
	JBoolean*			del
	)
	:
	itsIsSystemFlag(isSystem),
	itsNameRegex(NULL),
	itsContentRegex(NULL)
{
	input >> itsPattern >> itsCmd;

	if (vers < 2)
		{
		ConvertCommand(&itsCmd);
		JIgnoreLine(input);		// alternate cmd

		*isDefault = JI2B(itsPattern == kOrigDefaultMarker);
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

		input >> itsCmdType >> itsSingleFileFlag;
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
	delete itsNameRegex;
	delete itsContentRegex;
}

/******************************************************************************
 Match

 ******************************************************************************/

JBoolean
JFSBinding::Match
	(
	const JString& fileName,
	const JString& content
	)
	const
{
	if (itsCmd.IsEmpty())
		{
		return kJFalse;
		}
	else if (itsContentRegex != NULL)
		{
		return JI2B(content.BeginsWith(itsLiteralPrefix) &&
					itsContentRegex->Match(content));
		}
	else if (itsNameRegex != NULL)
		{
		return itsNameRegex->Match(fileName);
		}
	else
		{
		return JI2B(!itsPattern.IsEmpty() &&
					fileName.EndsWith(itsPattern, kJFalse));
		}
}

/******************************************************************************
 UpdateRegex (private)

 ******************************************************************************/

void
JFSBinding::UpdateRegex()
{
	if (itsPattern.BeginsWith(kContentRegexMarker))
		{
		delete itsNameRegex;
		itsNameRegex = NULL;

		if (itsContentRegex == NULL)
			{
			itsContentRegex = new JRegex;
			assert( itsContentRegex != NULL );
			itsContentRegex->SetSingleLine(kJTrue);
			}

		if (itsContentRegex->SetPattern(itsPattern).OK())
			{
			CalcLiteralPrefix();
			}
		else
			{
			delete itsContentRegex;
			itsContentRegex = NULL;
			}
		}
	else if (itsPattern.Contains(kNameRegexMarker))
		{
		delete itsContentRegex;
		itsContentRegex = NULL;

		if (itsNameRegex == NULL)
			{
			itsNameRegex = new JRegex;
			assert( itsNameRegex != NULL );
			}

		JString s;
		const JBoolean ok = JDirInfo::BuildRegexFromWildcardFilter(itsPattern, &s);
		assert( ok );
		if (!itsNameRegex->SetPattern(s).OK())
			{
			delete itsNameRegex;
			itsNameRegex = NULL;
			}
		}
	else
		{
		delete itsNameRegex;
		itsNameRegex = NULL;

		delete itsContentRegex;
		itsContentRegex = NULL;
		}
}

/******************************************************************************
 WillBeRegex (static)

 ******************************************************************************/

JBoolean
JFSBinding::WillBeRegex
	(
	const JString& pattern
	)
{
	return pattern.BeginsWith(kContentRegexMarker);
}

/******************************************************************************
 CalcLiteralPrefix (private)

 ******************************************************************************/

void
JFSBinding::CalcLiteralPrefix()
{
	const JCharacter* s = itsPattern.GetCString();

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

	itsLiteralPrefix = itsPattern.GetSubstring(JIndexRange(2, i));
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
	JIndex i=1;
	while (cmd->LocateNextSubstring("\"$f\"", &i))
		{
		cmd->ReplaceSubstring(i,i+3, "$q");
		}

	i=1;
	while (cmd->LocateNextSubstring("$f", &i))
		{
		cmd->ReplaceSubstring(i,i+1, "$u");
		}
}

/******************************************************************************
 GetCommandType (static)

 ******************************************************************************/

JFSBinding::CommandType
JFSBinding::GetCommandType
	(
	const JBoolean shell,
	const JBoolean window
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

JOrderedSetT::CompareResult
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

ostream&
operator<<
	(
	ostream&			output,
	const JFSBinding&	binding
	)
{
	JFSBinding::CommandType type;
	JBoolean singleFile;

	output << binding.GetPattern();
	output << ' ' << binding.GetCommand(&type, &singleFile);
	output << ' ' << type;
	output << ' ' << singleFile;
	return output;
}

/******************************************************************************
 Stream operators for JFSBinding::CommandType

 ******************************************************************************/

const JCharacter kPlainMarker  = 'p';
const JCharacter kShellMarker  = 's';
const JCharacter kWindowMarker = 'w';

istream&
operator>>
	(
	istream&					input,
	JFSBinding::CommandType&	type
	)
{
	input >> ws;

	JCharacter c;
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
		JSetState(input, ios::failbit);
		}

	return input;
}

ostream&
operator<<
	(
	ostream&						output,
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
