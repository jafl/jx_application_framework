/******************************************************************************
 CBSymbolList.cpp

	Uses ctags to maintain a list of symbols for a project.

	BASE CLASS = JContainer, CBCtagsUser

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBSymbolList.h"
#include "CBProjectDocument.h"
#include "CBFileListTable.h"
#include "CBCTreeDirector.h"
#include "CBCTree.h"
#include "CBCPreprocessor.h"
#include "cbGlobals.h"
#include "cbCtagsRegex.h"
#include <jStreamUtil.h>
#include <jDirUtil.h>
#include <strstream>
#include <jAssert.h>

static const JCharacter* kCtagsArgs =
	"--format=2 --excmd=number --sort=no --extras=q "
	"--c-kinds=+p --php-kinds=-v --ant-kinds=t --javascript-kinds=cfgm "
	CBCtagsBisonDef
	CBCtagsBisonNonterminalDef
	CBCtagsBisonNonterminalDecl
	CBCtagsBisonTerminalDecl
	CBCtagsHTMLDef
	CBCtagsLexDef
	CBCtagsLexState
	CBCtagsMakeDef
	CBCtagsMakeTarget
	CBCtagsMakeVariable;

// JBroadcaster message types

const JCharacter* CBSymbolList::kChanged = "Changed::CBSymbolList";

// string ID's

static const JCharacter* kCleaningUpID = "CleaningUp::CBSymbolList";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBSymbolList::CBSymbolList
	(
	CBProjectDocument* projDoc
	)
	:
	JContainer(),
	CBCtagsUser(kCtagsArgs)
{
	itsProjDoc                = projDoc;
	itsReparseAllFlag         = kJTrue;		// ReadSetup() clears this
	itsChangedDuringParseFlag = kJFalse;
	itsBeganEmptyFlag         = kJFalse;

	itsSymbolList = jnew JArray<SymbolInfo>(kBlockSize);
	assert( itsSymbolList != NULL );
	itsSymbolList->SetSortOrder(JOrderedSetT::kSortAscending);
	itsSymbolList->SetCompareFunction(CompareSymbols);

	InstallOrderedSet(itsSymbolList);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBSymbolList::~CBSymbolList()
{
	RemoveAllSymbols();
	jdelete itsSymbolList;
}

/******************************************************************************
 GetSymbol

	fullyQualifiedFileScope can be NULL.

 ******************************************************************************/

const JString&
CBSymbolList::GetSymbol
	(
	const JIndex	symbolIndex,
	CBLanguage*		lang,
	Type*			type,
	JBoolean*		fullyQualifiedFileScope
	)
	const
{
	const SymbolInfo info = itsSymbolList->GetElement(symbolIndex);
	*lang                 = info.lang;
	*type                 = info.type;

	if (fullyQualifiedFileScope != NULL)
		{
		*fullyQualifiedFileScope = info.fullyQualifiedFileScope;
		}

	return *(info.name);
}

/******************************************************************************
 GetFile

 ******************************************************************************/

const JString&
CBSymbolList::GetFile
	(
	const JIndex	symbolIndex,
	JIndex*			lineIndex
	)
	const
{
	const SymbolInfo info = itsSymbolList->GetElement(symbolIndex);
	*lineIndex            = info.lineIndex;
	return (itsProjDoc->GetAllFileList())->GetFileName(info.fileID);
}

/******************************************************************************
 IsUniqueClassName

	Returns kJTrue if the given name is the name of a single class.

 ******************************************************************************/

JBoolean
CBSymbolList::IsUniqueClassName
	(
	const JCharacter*	name,
	CBLanguage*			lang
	)
	const
{
	const JSize symCount  = itsSymbolList->GetElementCount();
	const SymbolInfo* sym = itsSymbolList->GetCArray();

	JBoolean found = kJFalse;
	for (JIndex i=0; i<symCount; i++)
		{
		if (IsClass(sym[i].type) &&
			JStringCompare(*(sym[i].name), name, CBIsCaseSensitive(sym[i].lang)) == 0)
			{
			if (!found)
				{
				found = kJTrue;
				*lang = sym[i].lang;
				}
			else
				{
				return kJFalse;
				}
			}
		}

	return found;
}

/******************************************************************************
 FindSymbol

	Fills matchList with the symbol indices that match.

	If contextFileID is valid, it is used for context.
	If contextNamespace is not empty, the names are used for context.

	*** The contents of *ContextNamespaceList are altered.

 ******************************************************************************/

class FindSymbolCompare : public JElementComparison<JIndex>
{
public:

	FindSymbolCompare(const JArray<CBSymbolList::SymbolInfo>& data)
		:
		itsData(data)
	{ };

	virtual ~FindSymbolCompare() { };

	virtual JOrderedSetT::CompareResult
	Compare(const JIndex& s1, const JIndex& s2) const
	{
		const CBSymbolList::SymbolInfo* info = itsData.GetCArray();
		return JCompareStringsCaseInsensitive(info[s1-1].name, info[s2-1].name);
	}

	virtual JElementComparison<JIndex>*
	Copy() const
	{
		FindSymbolCompare* copy = jnew FindSymbolCompare(itsData);
		assert( copy != NULL );
		return copy;
	}

private:

	const JArray<CBSymbolList::SymbolInfo>& itsData;
};

JBoolean
CBSymbolList::FindSymbol
	(
	const JCharacter*	name,
	const JFAID_t		contextFileID,
	const JString&		contextNamespace,
	const CBLanguage	contextLang,
	JPtrArray<JString>*	cContextNamespaceList,
	JPtrArray<JString>*	javaContextNamespaceList,
	JPtrArray<JString>*	phpContextNamespaceList,
	const JBoolean		findDeclaration,
	const JBoolean		findDefinition,
	JArray<JIndex>*		matchList
	)
	const
{
	// find all symbols that match

	matchList->RemoveAll();
	matchList->SetBlockSize(50);

	JArray<JIndex> allMatchList(50);

	JString s = name;
	SymbolInfo target;
	target.name = &s;
	JIndex startIndex;
	if (itsSymbolList->SearchSorted(target, JOrderedSetT::kFirstMatch, &startIndex))
		{
		const JSize count = itsSymbolList->GetElementCount();
		for (JIndex i=startIndex; i<=count; i++)
			{
			const SymbolInfo info = itsSymbolList->GetElement(i);
			if (CompareSymbols(target, info) != JOrderedSetT::kFirstEqualSecond)
				{
				break;
				}

			if (!CBIsCaseSensitive(info.lang) || *(info.name) == s)
				{
				if (contextFileID == info.fileID &&		// automatically fails if context is kInvalidID
					IsFileScope(info.type))
					{
					matchList->RemoveAll();
					matchList->AppendElement(i);
					break;
					}

				if ((findDeclaration || !IsPrototype(info.type)) &&
					(findDefinition  || !IsFunction(info.type)))
					{
					matchList->AppendElement(i);
					}
				allMatchList.AppendElement(i);
				}
			}
		}

	JBoolean usedAllList = kJFalse;
	if (matchList->IsEmpty())
		{
		*matchList  = allMatchList;
		usedAllList = kJTrue;
		}

	// replace symbols with fully qualified versions

	if (!matchList->IsEmpty())
		{
		JString ns1, ns2;
		PrepareContextNamespace(contextNamespace, contextLang, &ns1, &ns2);
		PrepareCContextNamespaceList(cContextNamespaceList);
		PrepareJavaContextNamespaceList(javaContextNamespaceList);
		PreparePHPContextNamespaceList(phpContextNamespaceList);

		JArray<JIndex> noContextList = *matchList;
		if (!ConvertToFullNames(&noContextList, matchList,
								ns1, ns2, contextLang,
								*cContextNamespaceList, *javaContextNamespaceList,
								*phpContextNamespaceList))
			{
			if (!usedAllList && !findDeclaration && findDefinition)
				{
				// if no definition, try declarations
				// (all: class decl, C++: pure virtual fn)

				JArray<JIndex> allNoContextList = allMatchList;
				if (ConvertToFullNames(&allNoContextList, &allMatchList,
									   ns1, ns2, contextLang,
									   *cContextNamespaceList, *javaContextNamespaceList,
									   *phpContextNamespaceList))
					{
					*matchList = allMatchList;
					}
				else
					{
					*matchList = noContextList;		// honor original request for only defn
					}
				}
			else
				{
				*matchList = noContextList;
				}
			}

		// re-sort

		matchList->SetCompareObject(FindSymbolCompare(*itsSymbolList));
		matchList->SetSortOrder(itsSymbolList->GetSortOrder());
		matchList->Sort();

		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 ConvertToFullNames (private)

	Replaces name with *.name (Eiffel, Java) or *:name (C++: file: or
	class::).  Returns kJFalse if contextList is empty.

 ******************************************************************************/

JBoolean
CBSymbolList::ConvertToFullNames
	(
	JArray<JIndex>*				noContextList,
	JArray<JIndex>*				contextList,
	const JString&				contextNamespace1,
	const JString&				contextNamespace2,
	const CBLanguage			contextLang,
	const JPtrArray<JString>&	cContextNamespaceList,
	const JPtrArray<JString>&	javaContextNamespaceList,
	const JPtrArray<JString>&	phpContextNamespaceList
	)
	const
{
	const JBoolean useLangNSContext = !contextNamespace1.IsEmpty();
	const JBoolean useCNSContext    = !cContextNamespaceList.IsEmpty();
	const JBoolean useJavaNSContext = !javaContextNamespaceList.IsEmpty();
	const JBoolean usePHPNSContext  = !phpContextNamespaceList.IsEmpty();

	// substitute indicies of fully qualified symbols
	// and filter based on namespace context

	const JSize symCount  = itsSymbolList->GetElementCount();
	const SymbolInfo* sym = itsSymbolList->GetCArray();

	const JSize count = noContextList->GetElementCount();
	JString s1, s2;
	for (JIndex i=count; i>=1; i--)
		{
		const JIndex j          = noContextList->GetElement(i) - 1;
		const SymbolInfo& info  = sym[j];
		const JBoolean caseSens = CBIsCaseSensitive(info.lang);

		s1 = "." + *(info.name);
		s2 = ":" + *(info.name);
		for (JIndex k=0; k<symCount; k++)
			{
			if (k != j &&
				sym[k].fileID    == info.fileID &&
				sym[k].lineIndex == info.lineIndex &&
				((sym[k].name)->EndsWith(s1, caseSens) ||
				 (sym[k].name)->EndsWith(s2, caseSens)))
				{
				if ((info.lang == kCBCLang && useCNSContext &&
					 !InContext(*(sym[k].name), cContextNamespaceList, kJTrue)) ||
					(info.lang == kCBJavaLang && useJavaNSContext &&
					 !InContext(*(sym[k].name), javaContextNamespaceList, kJTrue)) ||
					(info.lang == kCBPHPLang && usePHPNSContext &&
					 !InContext(*(sym[k].name), phpContextNamespaceList, kJTrue)) ||
					(info.lang == contextLang && useLangNSContext &&
					 !(sym[k].name)->BeginsWith(contextNamespace1, caseSens) &&
					 !(sym[k].name)->Contains(contextNamespace2, caseSens)))
					{
					contextList->RemoveElement(i);
					}
				else
					{
					contextList->SetElement(i, k+1);
					}

				noContextList->SetElement(i, k+1);
				break;
				}
			}
		}

	return !contextList->IsEmpty();
}

/******************************************************************************
 PrepareContextNamespace (private)

 ******************************************************************************/

void
CBSymbolList::PrepareContextNamespace
	(
	const JString&		contextNamespace,
	const CBLanguage	lang,
	JString*			ns1,
	JString*			ns2
	)
	const
{
	if (!contextNamespace.IsEmpty() &&
		(lang == kCBJavaLang       ||
		 lang == kCBJavaScriptLang ||
		 lang == kCBEiffelLang     ||
		 lang == kCBCSharpLang))
		{
		*ns1  = contextNamespace;	// name.
		*ns1 += ".";

		*ns2 = *ns1;				// .name.
		ns2->Prepend(".");
		}
	else
		{
		ns1->Clear();
		ns2->Clear();
		}
}

/******************************************************************************
 PrepareContextNamespaceList (private)

 ******************************************************************************/

void
CBSymbolList::PrepareCContextNamespaceList
	(
	JPtrArray<JString>* contextNamespace
	)
	const
{
	PrepareContextNamespaceList(contextNamespace, "::");
}

void
CBSymbolList::PrepareJavaContextNamespaceList
	(
	JPtrArray<JString>* contextNamespace
	)
	const
{
	PrepareContextNamespaceList(contextNamespace, ".");
}

void
CBSymbolList::PreparePHPContextNamespaceList
	(
	JPtrArray<JString>* contextNamespace
	)
	const
{
	PrepareContextNamespaceList(contextNamespace, "\\");
}

void
CBSymbolList::PrepareContextNamespaceList
	(
	JPtrArray<JString>*	contextNamespace,
	const JCharacter*	namespaceOp
	)
	const
{
	if (!contextNamespace->IsEmpty())
		{
		const JSize count = contextNamespace->GetElementCount();
		for (JIndex i=count; i>=1; i--)
			{
			JString* cns1 = contextNamespace->NthElement(i);	// name::
			*cns1 += namespaceOp;

			JString* cns2 = jnew JString(*cns1);					// ::name::
			assert( cns2 != NULL );
			cns2->Prepend(namespaceOp);
			contextNamespace->InsertAtIndex(i+1, cns2);
			}
		}
}

/******************************************************************************
 InContext (private)

 ******************************************************************************/

JBoolean
CBSymbolList::InContext
	(
	const JString&				fullName,
	const JPtrArray<JString>&	contextNamespace,
	const JBoolean				caseSensitive
	)
	const
{
	const JSize count = contextNamespace.GetElementCount();
	for (JIndex i=1; i<=count; i+=2)
		{
		const JString* cns1 = contextNamespace.NthElement(i);
		const JString* cns2 = contextNamespace.NthElement(i+1);
		if (fullName.BeginsWith(*cns1, caseSensitive) ||
			fullName.Contains(*cns2, caseSensitive))
			{
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 ClosestMatch

	Returns the index of the closest match for the given name prefix,
	searching only the items in visibleList.

	This is rather ugly.  We search for zero, which can't be in the list,
	and the compare function uses this to figure out which item is the
	target and which is an index into the full symbol list.

 ******************************************************************************/

class ClosestMatchCompare : public JElementComparison<JIndex>
{
public:

	ClosestMatchCompare(const JString& prefix, const JArray<CBSymbolList::SymbolInfo>& data)
		:
		itsPrefix(prefix), itsData(data)
	{ };

	virtual ~ClosestMatchCompare() { };

	virtual JOrderedSetT::CompareResult
	Compare(const JIndex& s1, const JIndex& s2) const
	{
		JString* prefix = const_cast<JString*>(&itsPrefix);
		if (s1 == 0)
			{
			const CBSymbolList::SymbolInfo& info = (itsData.GetCArray())[s2-1];
			return JCompareStringsCaseInsensitive(prefix, info.name);
			}
		else if (s2 == 0)
			{
			const CBSymbolList::SymbolInfo& info = (itsData.GetCArray())[s1-1];
			return JCompareStringsCaseInsensitive(info.name, prefix);
			}
		else
			{
			assert_msg( 0, "CBSymbolList.cc:ClosestMatchCompare::Compare() didn't get a zero" );
			return JOrderedSetT::kFirstEqualSecond;
			}
	}

	virtual JElementComparison<JIndex>*
	Copy() const
	{
		ClosestMatchCompare* copy = jnew ClosestMatchCompare(itsPrefix, itsData);
		assert( copy != NULL );
		return copy;
	}

private:

	const JString&							itsPrefix;
	const JArray<CBSymbolList::SymbolInfo>&	itsData;
};

JBoolean
CBSymbolList::ClosestMatch
	(
	const JString&	prefixStr,
	JArray<JIndex>&	visibleList,
	JIndex*			index
	)
	const
{
	visibleList.SetCompareObject(ClosestMatchCompare(prefixStr, *itsSymbolList));
	visibleList.SetSortOrder(itsSymbolList->GetSortOrder());

	JBoolean found;
	*index = visibleList.SearchSorted1(0, JOrderedSetT::kFirstMatch, &found);
	if (*index > visibleList.GetElementCount())		// insert beyond end of list
		{
		*index = visibleList.GetElementCount();
		}

	return JConvertToBoolean( *index > 0 );
}

/******************************************************************************
 FileTypesChanged

	We can't use Receive() because this must be called -before-
	CBProjectDocument updates everything.

 ******************************************************************************/

void
CBSymbolList::FileTypesChanged
	(
	const CBPrefsManager::FileTypesChanged& info
	)
{
	if (info.Changed(IsParsed))
		{
		itsReparseAllFlag = kJTrue;
		}
}

/******************************************************************************
 PrepareForUpdate

	Get ready to parse files that have changed or been created and to
	throw out symbols in files that no longer exist.

	*** This often runs in the update thread.

 ******************************************************************************/

void
CBSymbolList::PrepareForUpdate
	(
	const JBoolean		reparseAll,
	JProgressDisplay&	pg
	)
{
	assert( !itsReparseAllFlag || reparseAll );

	if (reparseAll)
		{
		RemoveAllSymbols();
		}
	itsChangedDuringParseFlag = reparseAll;

	// sort more strictly when building the list

	itsSymbolList->SetCompareFunction(CompareSymbolsAndTypes);

	itsBeganEmptyFlag = itsSymbolList->IsEmpty();
}

/******************************************************************************
 UpdateFinished

	Cleans up after updating files.

	*** This often runs in the update thread.

 ******************************************************************************/

JBoolean
CBSymbolList::UpdateFinished
	(
	const JArray<JFAID_t>&	deadFileList,
	JProgressDisplay&		pg
	)
{
	DeleteProcess();

	// reset to lenient search

	itsSymbolList->SetCompareFunction(CompareSymbols);

	// toss files that no longer exist

	const JSize fileCount = deadFileList.GetElementCount();
	if (fileCount > 0)
		{
		pg.FixedLengthProcessBeginning(fileCount, JGetString(kCleaningUpID), kJFalse, kJTrue);

		for (JIndex i=1; i<=fileCount; i++)
			{
			RemoveFile(deadFileList.GetElement(i));
			pg.IncrementProgress();
			}

		pg.ProcessFinished();
		}

	if (itsChangedDuringParseFlag && !CBInUpdateThread())
		{
		itsReparseAllFlag = kJFalse;
		Broadcast(Changed());
		}

	return itsChangedDuringParseFlag;
}

/******************************************************************************
 RemoveAllSymbols (private)

 ******************************************************************************/

void
CBSymbolList::RemoveAllSymbols()
{
	const JSize count = itsSymbolList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		SymbolInfo info = itsSymbolList->GetElement(i);
		info.Free();
		}
	itsSymbolList->RemoveAll();
}

/******************************************************************************
 RemoveFile (private)

	Throws out all symbols that were defined in the given file.

 ******************************************************************************/

void
CBSymbolList::RemoveFile
	(
	const JIndex id
	)
{
	const JSize count = itsSymbolList->GetElementCount();
	for (JIndex i=count; i>=1; i--)
		{
		SymbolInfo info = itsSymbolList->GetElement(i);
		if (info.fileID == id)
			{
			info.Free();
			itsSymbolList->RemoveElement(i);
			itsChangedDuringParseFlag = kJTrue;
			}
		}
}

/******************************************************************************
 FileChanged

	Throws out all symbols that were in the given file and reparses it.

 ******************************************************************************/

void
CBSymbolList::FileChanged
	(
	const JCharacter*		fileName,
	const CBTextFileType	fileType,
	const JFAID_t			id
	)
{
	if (IsParsed(fileType))
		{
		if (!itsBeganEmptyFlag)
			{
			RemoveFile(id);
			}
		ParseFile(fileName, fileType, id);
		}
}

/******************************************************************************
 ParseFile (private)

	Runs the specified file through ctags.

 ******************************************************************************/

void
CBSymbolList::ParseFile
	(
	const JCharacter*		fileName,
	const CBTextFileType	fileType,
	const JFAID_t			id
	)
{
	JString data;
	CBLanguage lang;
	if (ProcessFile(fileName, fileType, &data, &lang))
		{
		std::istrstream input(data.GetCString(), data.GetLength());
		ReadSymbolList(input, lang, fileName, id);
		itsChangedDuringParseFlag = kJTrue;
		}
}

/******************************************************************************
 InitCtags (virtual protected)

	C preprocessor macros only need to be defined once.

 ******************************************************************************/

void
CBSymbolList::InitCtags
	(
	std::ostream& output
	)
{
	(((itsProjDoc->GetCTreeDirector())->
		GetCTree())->GetCPreprocessor())->PrintMacrosForCTags(output);
}

/******************************************************************************
 ReadSymbolList (private)

 ******************************************************************************/

void
CBSymbolList::ReadSymbolList
	(
	std::istream&			input,
	const CBLanguage	lang,
	const JCharacter*	fullName,
	const JFAID_t		fileID
	)
{
	JString path, fileName;
	JSplitPathAndName(fullName, &path, &fileName);

	input >> std::ws;
	while (input.peek() == '!')
		{
		JIgnoreLine(input);
		input >> std::ws;
		}

	JStringPtrMap<JString> flags(JPtrArrayT::kDeleteAll);
	while (1)
		{
		JString* name = jnew JString;
		assert( name != NULL );

		input >> std::ws;
		*name = JReadUntil(input, '\t');		// symbol name
		if (input.eof() || input.fail())
			{
			jdelete name;
			break;
			}

		JIgnoreUntil(input, '\t');				// file name

		JIndex lineIndex;
		input >> lineIndex;						// line index

		ReadExtensionFlags(input, &flags);

		JCharacter typeChar = ' ';
		JString* value;
		if (flags.GetElement("kind", &value) && !value->IsEmpty())
			{
			typeChar = value->GetFirstCharacter();
			}

		JString* signature = NULL;
		if (flags.GetElement("signature", &value) && !value->IsEmpty())
			{
			signature = jnew JString(*value);
			assert( signature != NULL );
			signature->PrependCharacter(' ');
			}

		if (IgnoreSymbol(*name))
			{
			jdelete name;
			}
		else
			{
			const Type type = DecodeSymbolType(lang, typeChar);
			if (signature == NULL &&
				(IsFunction(type) || IsPrototype(type)))
				{
				signature = jnew JString(" ( )");
				assert( signature != NULL );
				}

			const SymbolInfo info(name, signature, lang, type,
								  kJFalse, fileID, lineIndex);
			itsSymbolList->InsertSorted(info);

			// add file:name

			if (IsFileScope(type))
				{
				JString* name1 = jnew JString(fileName);
				assert( name1 != NULL );
				*name1 += ":";
				*name1 += *name;

				JString* sig1 = NULL;
				if (signature != NULL)
					{
					sig1 = jnew JString(*signature);
					assert( sig1 != NULL );
					}

				const SymbolInfo info1(name1, sig1, lang, type,
									   kJTrue, fileID, lineIndex);
				itsSymbolList->InsertSorted(info1);
				}
			}
		}
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
CBSymbolList::ReadSetup
	(
	std::istream&			projInput,
	const JFileVersion	projVers,
	std::istream*			symInput,
	const JFileVersion	symVers
	)
{
	std::istream* input          = (projVers <= 41 ? &projInput : symInput);
	const JFileVersion vers = (projVers <= 41 ? projVers   : symVers);
	if (input != NULL)
		{
		ReadSetup(*input, vers);

		itsReparseAllFlag = JI2B(
			vers < 83 || (itsSymbolList->IsEmpty() && IsActive()));
		}
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
CBSymbolList::ReadSetup
	(
	std::istream&			input,
	const JFileVersion	vers
	)
{
	RemoveAllSymbols();
	itsReparseAllFlag = kJFalse;

	JSize symbolCount;
	input >> symbolCount;

	itsSymbolList->SetBlockSize(symbolCount+1);		// avoid repeated realloc

	for (JIndex i=1; i<=symbolCount; i++)
		{
		JString* name = jnew JString;
		assert( name != NULL );
		input >> *name;

		long lang, type;
		input >> lang >> type;

		JBoolean fullyQualifiedFileScope = kJFalse;
		if (vers >= 54)
			{
			input >> fullyQualifiedFileScope;
			}

		JFAID_t fileID;
		JIndex lineIndex;
		input >> fileID >> lineIndex;

		JString* signature = NULL;
		if (vers > 63)
			{
			JBoolean hasSignature;
			input >> hasSignature;

			if (hasSignature)
				{
				signature = jnew JString;
				assert( signature != NULL );
				input >> *signature;
				}
			}

		itsSymbolList->AppendElement(
			SymbolInfo(name, signature, (CBLanguage) lang, (Type) type,
					   fullyQualifiedFileScope, fileID, lineIndex));
		}

	itsSymbolList->SetBlockSize(kBlockSize);

	if (vers < 51)
		{
		itsSymbolList->SetCompareFunction(CompareSymbolsAndTypes);
		itsSymbolList->Sort();
		itsSymbolList->SetCompareFunction(CompareSymbols);
		}

	Broadcast(Changed());
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
CBSymbolList::WriteSetup
	(
	std::ostream& projOutput,
	std::ostream* symOutput
	)
	const
{
	if (symOutput != NULL)
		{
		const JSize symbolCount = itsSymbolList->GetElementCount();
		*symOutput << ' ' << symbolCount;

		for (JIndex i=1; i<=symbolCount; i++)
			{
			const SymbolInfo info = itsSymbolList->GetElement(i);
			*symOutput << ' ' << *(info.name);
			*symOutput << ' ' << (long) info.lang;
			*symOutput << ' ' << (long) info.type;
			*symOutput << ' ' << info.fullyQualifiedFileScope;
			*symOutput << ' ' << info.fileID;
			*symOutput << ' ' << info.lineIndex;

			if (info.signature != NULL)
				{
				*symOutput << ' ' << kJTrue;
				*symOutput << ' ' << *(info.signature);
				}
			else
				{
				*symOutput << ' ' << kJFalse;
				}
			}

		*symOutput << ' ';
		}
}

/******************************************************************************
 Receive (virtual protected)

	Required because of multiple inheritance.

 ******************************************************************************/

void
CBSymbolList::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JContainer::Receive(sender, message);
	CBCtagsUser::Receive(sender, message);
}

/******************************************************************************
 CompareSymbols (static)

 ******************************************************************************/

JOrderedSetT::CompareResult
CBSymbolList::CompareSymbols
	(
	const SymbolInfo& s1,
	const SymbolInfo& s2
	)
{
	return JCompareStringsCaseInsensitive(s1.name, s2.name);
}

/******************************************************************************
 CompareSymbolsAndTypes (static)

 ******************************************************************************/

JOrderedSetT::CompareResult
CBSymbolList::CompareSymbolsAndTypes
	(
	const SymbolInfo& s1,
	const SymbolInfo& s2
	)
{
	const JOrderedSetT::CompareResult result =
		JCompareStringsCaseInsensitive(s1.name, s2.name);

	if (result == JOrderedSetT::kFirstEqualSecond)
		{
		if (s1.type < s2.type)
			{
			return JOrderedSetT::kFirstLessSecond;
			}
		else if (s1.type == s2.type)
			{
			return JOrderedSetT::kFirstEqualSecond;
			}
		else
			{
			return JOrderedSetT::kFirstGreaterSecond;
			}
		}
	else
		{
		return result;
		}
}

/******************************************************************************
 SymbolInfo functions (private)

 ******************************************************************************/

void
CBSymbolList::SymbolInfo::Free()
{
	jdelete name;
	name = NULL;
}
