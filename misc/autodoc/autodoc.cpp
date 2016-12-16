/******************************************************************************
 autodoc.cpp

	Autodoc is the JX documentation builder which creates *.doc files
	from the corresponding source and header files.

	Copyright (C) 1996 by Dustin Laurence.
	Copyright (C) 1999 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include <JString.h>
#include <jStreamUtil.h>
#include <jFStreamUtil.h>
#include <jFileUtil.h>
#include <jAssert.h>

// Constants

static const JCharacter* kDocExtension = ".doc";

static const JCharacter* kSourceExtension[] =
{
	".cc", ".cpp", ".cxx", ".C", ".tmpl"
};
const JSize kSourceExtCount = sizeof(kSourceExtension)/sizeof(JCharacter*);

static const JCharacter* kHeaderExtension[] =
{
	".h", ".hh", ".hpp", ".hxx", ".H"
};
const JSize kHeaderExtCount = sizeof(kHeaderExtension)/sizeof(JCharacter*);

// Prototypes

JBoolean ShouldDoc(const JString& fileName, JString* fileBase, JString* complName);
void     WriteDoc(std::istream& cFile, const JCharacter* docFileName, std::ofstream& docFile,
				  JBoolean* wrote);

/******************************************************************************
 main

 *****************************************************************************/

int
main
	(
	int		argc,
	char*	argv[]
	)
{
	JString nameBase, complName;
	for (JIndex i=1; i < (JIndex) argc; i++)
		{
		const JString thisName = argv[i];
		if (ShouldDoc(thisName, &nameBase, &complName))
			{
			const JString docName = nameBase + kDocExtension;
			std::ofstream docFile;

			JBoolean wrote = kJFalse;

			std::ifstream cFile(thisName);
			if (cFile.good())
				{
				WriteDoc(cFile, docName, docFile, &wrote);
				}
			else
				{
				std::cerr << "autodoc: can't read file \"";
				std::cerr << thisName << '\"' << std::endl;
				continue;
				}

			if (!complName.IsEmpty())
				{
				std::ifstream hFile(complName);
				if (hFile.good())
					{
					WriteDoc(hFile, docName, docFile, &wrote);
					}
				else
					{
					std::cerr << "autodoc: can't read file \"";
					std::cerr << complName << '\"' << std::endl;
					continue;
					}
				}
			}
		}

	return 0;
}

/******************************************************************************
 ShouldDoc

	Returns kJTrue if fileName has the correct extension for a source file.
	*fileBase then contains the file name with the extension stripped off.
	*complName then contains the name of the corresponding header file, or
	it is empty if it doesn't exist.

 *****************************************************************************/

JBoolean
ShouldDoc
	(
	const JString&	fileName,
	JString*		fileBase,
	JString*		complName
	)
{
	const JSize nameLength = fileName.GetLength();
	for (JIndex i=1; i<=kSourceExtCount; i++)
		{
		if (fileName.EndsWith(kSourceExtension[i-1]))
			{
			*fileBase = fileName.GetSubstring(1,
							nameLength - strlen(kSourceExtension[i-1]));

			for (JIndex j=1; j<=kHeaderExtCount; j++)
				{
				*complName = *fileBase + kHeaderExtension[j-1];
				if (JFileExists(*complName))
					{
					break;
					}
				complName->Clear();
				}
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 WriteDoc

	Writes all the lines in cFile between the document delimiters to docFile.

	We only open the file when we find something to write.  This avoids
	generating lots of empty files.

	We do not use std::endl because that flushes the stream, which slows us
	down significantly when doing an entire directory of files.

 *****************************************************************************/

void
WriteDoc
	(
	std::istream&			cFile,
	const JCharacter*	docFileName,
	std::ofstream&			docFile,
	JBoolean*			wrote
	)
{
	JBoolean writing = kJFalse;

	while (cFile.good())
		{
		const JString thisLine = JReadLine(cFile);

		if (!writing && thisLine.BeginsWith("/********************"))
			{
			if (!docFile.is_open())
				{
				docFile.open(docFileName);
				if (!docFile.good())
					{
					std::cerr << "autodoc: can't write file \"";
					std::cerr << docFileName << "\"" << std::endl;
					return;
					}
				}

			if (*wrote)
				{
				docFile << "----------------------------------------\n";
				}
			writing = *wrote = kJTrue;
			}

		else if (writing && thisLine.EndsWith("********************/"))
			{
			writing = kJFalse;
			}

		else if (writing)
			{
			assert( docFile.is_open() );
			thisLine.Print(docFile);
			docFile << '\n';
			}
		}
}
