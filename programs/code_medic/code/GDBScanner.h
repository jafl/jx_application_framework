/******************************************************************************
 GDBScanner.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_GDBScanner
#define _H_GDBScanner

#ifndef _H_GDBScannerL
#undef yyFlexLexer
#define yyFlexLexer GDBOutputFlexLexer
#include <FlexLexer.h>
#endif

#include "CMLocation.h"
#include "CMFlexLexerHacks.h"

class GDBScanner : public GDBOutputFlexLexer
{
public:

	enum TokenType
	{
		kEOF = 258,
		kProgramOutput,
		kErrorOutput,
		kCommandOutput,
		kCommandResult,
		kDebugOutput,

		kMaybeReadyForInput,	// OSX
		kReadyForInput,
		kDebuggerFinished,

		kPrepareToLoadSymbols,
		kFinishedLoadingSymbolsFromProgram,
		kNoSymbolsInProgram,
		kSymbolsReloaded,

		kCoreChanged,

		kAttachedToProcess,
		kDetachingFromProcess,

		kProgramStarting,
		kBeginGetPID,
		kProgramPID,
		kEndGetPID,

		kProgramStoppedAtLocation,
		kProgramRunning,
		kProgramFinished,
		kProgramKilled,

		kBeginMedicCmd,
		kEndMedicCmd,

		kBeginMedicIgnoreCmd,
		kEndMedicIgnoreCmd,

		kBreakpointsChanged,
		kFrameChanged,
		kThreadChanged,
		kValueChanged,

		kFrameChangedAndProgramStoppedAtLocation,

		kBeginScriptDefinition,
		kPlugInMessage
	};

	struct Data
	{
		JUInt		number;
		CMLocation*	pLocation;
		JString*	pString;
	};

	struct Token
	{
		TokenType	type;
		Data		data;

		Token()
			:
			type(kProgramOutput)
			{ };

		Token(const TokenType t, const Data& d)
			:
			type(t), data(d)
			{ };
	};

public:

	GDBScanner();
	virtual ~GDBScanner();

	void	Reset();
	void	AppendInput(const JString& string);

	Token	NextToken();		// written by flex

	static void	TranslateMIOutput(JString* data);

protected:

	virtual FlexLexerInputSize	LexerInput(char* buf, FlexLexerInputSize max_size) override;

private:

	JString	itsPartialBuffer;
	JString	itsInputBuffer;
	JSize	itsBytesRead;

	bool	itsResetFlag;

	// "Registers" -- temporary buffers for use in Token
	// (not in .l file to allow multiple instances)

	Data			itsData;
	CMLocation		itsLocation;
	JString			itsString;

private:

	void	Flush();
	void	ExtractCommandId(const JUtf8Byte* yytext, const JSize yyleng);

	// not allowed

	GDBScanner(const GDBScanner& source);
	const GDBScanner& operator=(const GDBScanner& source);
};

#endif
