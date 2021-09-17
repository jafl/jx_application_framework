/******************************************************************************
 CBSymbolList.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBSymbolList
#define _H_CBSymbolList

#include <JContainer.h>
#include "CBCtagsUser.h"
#include "CBPrefsManager.h"		// need definition of FileTypesChanged
#include <JFAID.h>
#include <JArray.h>

class JProgressDisplay;
class CBProjectDocument;

class CBSymbolList : public JContainer, public CBCtagsUser
{
public:

	enum
	{
		kBlockSize = 2048
	};

public:

	CBSymbolList(CBProjectDocument* projDoc);

	virtual ~CBSymbolList();

	const JString&	GetSymbol(const JIndex symbolIndex,
							  CBLanguage* lang, Type* type,
							  bool* fullyQualifiedFileScope = nullptr) const;
	const JString&	GetFile(const JIndex symbolIndex, JIndex* lineIndex) const;
	bool		GetSignature(const JIndex symbolIndex,
								 const JString** signature) const;

	bool	IsUniqueClassName(const JString& name, CBLanguage* lang) const;
	bool	FindSymbol(const JString& name, const JFAID_t contextFileID,
						   const JString& contextNamespace, const CBLanguage contextLang,
						   JPtrArray<JString>* cContextNamespaceList,
						   JPtrArray<JString>* dContextNamespaceList,
						   JPtrArray<JString>* goContextNamespaceList,
						   JPtrArray<JString>* javaContextNamespaceList,
						   JPtrArray<JString>* phpContextNamespaceList,
						   const bool findDeclaration, const bool findDefinition,
						   JArray<JIndex>* matchList) const;
	bool	ClosestMatch(const JString& prefixStr,
							 JArray<JIndex>& visibleList, JIndex* index) const;

	void	ReadSetup(std::istream& projInput, const JFileVersion projVers,
					  std::istream* symInput, const JFileVersion symVers);
	void	WriteSetup(std::ostream& projOutput, std::ostream* symOutput) const;

	// for loading updated symbols

	void	ReadSetup(std::istream& input, const JFileVersion vers);

	// called by CBSymbolDirector

	void		FileTypesChanged(const CBPrefsManager::FileTypesChanged& info);
	void		PrepareForUpdate(const bool reparseAll, JProgressDisplay& pg);
	bool	UpdateFinished(const JArray<JFAID_t>& deadFileList, JProgressDisplay& pg);

	// called by CBFileListTable

	bool	NeedsReparseAll() const;
	void		FileChanged(const JString& fileName,
							const CBTextFileType fileType, const JFAID_t id);

protected:

	virtual void	InitCtags(std::ostream& output) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

public:		// ought to be private

	struct SymbolInfo
	{
		JString*	name;		// shared with CBStringCompleter!
		JString*	signature;	// can be nullptr
		CBLanguage	lang;
		Type		type;
		bool		fullyQualifiedFileScope;
		JFAID_t		fileID;
		JIndex		lineIndex;

		SymbolInfo()
			:
			name(nullptr), signature(nullptr), lang(kCBOtherLang), type(kUnknownST),
			fullyQualifiedFileScope(false),
			fileID(JFAID::kInvalidID), lineIndex(0)
		{ };

		SymbolInfo(JString* n, JString* s, const CBLanguage l, const Type t,
				   const bool fqfs, const JFAID_t id, const JIndex line)
			:
			name(n), signature(s), lang(l), type(t),
			fullyQualifiedFileScope(fqfs),
			fileID(id), lineIndex(line)
		{ };

		void Free();
	};

private:

	CBProjectDocument*	itsProjDoc;					// not owned
	JArray<SymbolInfo>*	itsSymbolList;
	bool			itsReparseAllFlag;			// true => flush all on next update
	bool			itsChangedDuringParseFlag;
	bool			itsBeganEmptyFlag;			// true => ignore RemoveFile()

private:

	void	RemoveAllSymbols();
	void	RemoveFile(const JIndex id);

	void	ParseFile(const JString& fileName,
					  const CBTextFileType fileType, const JFAID_t id);
	void	ReadSymbolList(std::istream& input, const CBLanguage lang,
						   const JString& fileName, const JFAID_t fileID);

	bool	ConvertToFullNames(JArray<JIndex>* noContextList,
								   JArray<JIndex>* contextList,
								   const JString& contextNamespace1,
								   const JString& contextNamespace2,
								   const CBLanguage contextLang,
								   const JPtrArray<JString>& cContextNamespace,
								   const JPtrArray<JString>& dContextNamespace,
								   const JPtrArray<JString>& goContextNamespace,
								   const JPtrArray<JString>& javaContextNamespaceList,
								   const JPtrArray<JString>& phpContextNamespaceList) const;
	void		PrepareContextNamespace(const JString& contextNamespace, const CBLanguage lang,
										JString* ns1, JString* ns2) const;
	void		PrepareCContextNamespaceList(JPtrArray<JString>* contextNamespace) const;
	void		PrepareDContextNamespaceList(JPtrArray<JString>* contextNamespace) const;
	void		PrepareGoContextNamespaceList(JPtrArray<JString>* contextNamespace) const;
	void		PrepareJavaContextNamespaceList(JPtrArray<JString>* contextNamespace) const;
	void		PreparePHPContextNamespaceList(JPtrArray<JString>* contextNamespace) const;
	void		PrepareContextNamespaceList(JPtrArray<JString>* contextNamespace,
											const JUtf8Byte* namespaceOp) const;
	bool	InContext(const JString& fullName,
						  const JPtrArray<JString>& contextNamespace,
						  const JString::Case caseSensitive) const;

	static JListT::CompareResult
	CompareSymbols(const SymbolInfo& s1, const SymbolInfo& s2);

	static JListT::CompareResult
	CompareSymbolsAndTypes(const SymbolInfo& s1, const SymbolInfo& s2);

	// not allowed

	CBSymbolList(const CBSymbolList& source);
	const CBSymbolList& operator=(const CBSymbolList& source);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kChanged;

	class Changed : public JBroadcaster::Message
		{
		public:

			Changed()
				:
				JBroadcaster::Message(kChanged)
				{ };
		};
};


/******************************************************************************
 NeedsReparseAll

 ******************************************************************************/

inline bool
CBSymbolList::NeedsReparseAll()
	const
{
	return itsReparseAllFlag;
}

/******************************************************************************
 GetSignature

 ******************************************************************************/

inline bool
CBSymbolList::GetSignature
	(
	const JIndex	symbolIndex,
	const JString**	signature
	)
	const
{
	const SymbolInfo info = itsSymbolList->GetElement(symbolIndex);
	*signature = info.signature;
	return info.signature != nullptr;
}

#endif
