/******************************************************************************
 JMathematicaSM.cpp

	This class implements the JSymbolicMath interface to Mathematica¨.

	BASE CLASS = JSymbolicMath

	Copyright © 1995-96 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JMathematicaSM.h>
#include <jParseFunction.h>
#include <JFunction.h>
#include <JString.h>
#include <jTime.h>
#include <jGlobals.h>
#include <jAssert.h>

// #define HAS_MATHLINK		// for checking code without mathlink

/******************************************************************************
 Constructor function (static)

	By forcing everyone to use this function, we avoid having to worry
	about MLOpen() succeeding within the class itself.

	Note that this prevents one from creating derived classes.

 ******************************************************************************/

JBoolean
JMathematicaSM::Create
	(
	JMathematicaSM** obj
	)
{
	*obj = NULL;

#if !defined HAS_MATHLINK && !defined FAKE_MATHLINK

	return kJFalse;

#elif defined FAKE_MATHLINK

	*obj = new JMathematicaSM(NULL, NULL);
	return JConvertToBoolean( *obj != NULL );

#else

	MLEnvironment env = MLInitialize((MLParametersPointer) NULL);
	if (env == NULL)
		{
		(JGetUserNotification())->ReportError("Unable to initialize Mathematica.");
		return kJFalse;
		}

	// need fake argc and argv to start mathlink

	#ifdef _J_UNIX

		const int argc = 3;
		char* argv[argc];
		argv[0] = "dummy";
		argv[1] = "-linkname";
		argv[2] = "math -mathlink";

	#elif defined _MSC_VER

		// Having to hard-code the path is evil!

		const int argc = 3;
		char* argv[argc];
		argv[0] = "dummy";
		argv[1] = "-linkname";
		argv[2] = "C:/Program Files/Wolfram Research/Mathematica/3.0/MathKernel";
//		argv[2] = "math -mathlink";

	#else

		figure out what to do!

	#endif

	#ifdef USE_MATHLINK2

		MLINK link = MLOpen(argc, argv);

	#else

		long err;
		MLINK link = MLOpenArgv(env, argv, argv+argc, &err);

	#endif

	if (link == NULL)
		{
		(JGetUserNotification())->ReportError("Unable to connect to Mathematica.");
		MLDeinitialize(env);
		return kJFalse;
		}
	else
		{
		*obj = new JMathematicaSM(env, link);
		return JConvertToBoolean( *obj != NULL );
		}

#endif
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JMathematicaSM::JMathematicaSM
	(
	MLEnvironment	env,
	MLINK			link
	)
	:
	JSymbolicMath(),
	itsEnv(env),
	itsLink(link)
{
#ifdef HAS_MATHLINK

	assert( itsEnv != NULL && itsLink != NULL );

#endif
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JMathematicaSM::~JMathematicaSM()
{
#ifdef HAS_MATHLINK

	MLClose(itsLink);
	MLDeinitialize(itsEnv);

#endif
}

/******************************************************************************
 Print

 ******************************************************************************/

JString
JMathematicaSM::Print
	(
	const JFunction& f
	)
	const
{
	return f.PrintForMathematica();
}

/******************************************************************************
 SendSysCmd

 ******************************************************************************/

void
JMathematicaSM::SendSysCmd
	(
	const JCharacter* cmd
	)
	const
{
	SendText(cmd);
}

/******************************************************************************
 Simplify

	Generate Mathematica command to simplify a function.

	Simplify[ function ]

 ******************************************************************************/

JBoolean
JMathematicaSM::Simplify
	(
	const JFunction*		f,
	const JVariableList*	varList,
	JFunction**				result
	)
	const
{
	const JString str = ConvertToMathematica(f);
	JString resultStr;
	if (Simplify(str, &resultStr))
		{
		return JParseFunction(resultStr, varList, result);
		}
	else
		{
		return kJFalse;
		}
}


JBoolean
JMathematicaSM::Simplify
	(
	const JCharacter*	function,
	JString*			result
	)
	const
{
	JString cmd = "Simplify[ ";
	cmd += function;
	cmd += " ]";

	JString outputName;
	const JBoolean ok = SendRecvText(cmd, &outputName, result);
	if (ok)
		{
		ConvertFromMathematica(result);
		}

//	cout << endl << *result << endl;

	return ok;
}

/******************************************************************************
 Evaluate

	Generate Mathematica command to evaluate a function based on the
	previous result.

	Simplify[ ReplaceAll[ function, rules ]]

 ******************************************************************************/

JBoolean
JMathematicaSM::Evaluate
	(
	const JFunction*		f,
	const JCharacter*		values,
	const JVariableList*	varList,
	JFunction**				result
	)
	const
{
	const JString str = ConvertToMathematica(f);
	JString resultStr;
	if (Evaluate(str, values, &resultStr))
		{
		return JParseFunction(resultStr, varList, result);
		}
	else
		{
		return kJFalse;
		}
}


JBoolean
JMathematicaSM::Evaluate
	(
	const JCharacter*	function,
	const JCharacter*	values,
	JString*			result
	)
	const
{
	JString cmd = "Simplify[ ReplaceAll[ ";
	cmd += function;
	cmd += ", ";
	cmd += values;
	cmd += " ]]";

	JString outputName;
	const JBoolean ok = SendRecvText(cmd, &outputName, result);
	if (ok)
		{
		ConvertFromMathematica(result);
		}

//	cout << endl << *result << endl;

	return ok;
}

/******************************************************************************
 SolveEquations

	Generate Mathematica command to solve the given set of equations for
	the given variables.

	Simplify[ Solve[ {equations}, {variables} ]]

 ******************************************************************************/

JBoolean
JMathematicaSM::SolveEquations
	(
	const JPtrArray<JFunction>&	lhs,
	const JPtrArray<JFunction>&	rhs,
	const JPtrArray<JString>&	variables,
	JString*					result
	)
	const
{
	// build equations the way Mathematica wants them

	const JSize eqCount = lhs.GetElementCount();
	assert( eqCount == rhs.GetElementCount() );

	JString equations;
	for (JIndex i=1; i<=eqCount; i++)
		{
		const JString lhsStr = ConvertToMathematica(lhs.NthElement(i));
		const JString rhsStr = ConvertToMathematica(rhs.NthElement(i));
		equations += lhsStr;  
		equations += " == ";
		equations += rhsStr;
		if (i < eqCount)
			{
			equations += ", ";
			}
		}

	return PrivateSolveEqns(equations, variables, result);
}


JBoolean
JMathematicaSM::SolveEquations
	(
	const JPtrArray<JString>&	lhs,
	const JPtrArray<JString>&	rhs,
	const JPtrArray<JString>&	variables,
	JString*					result
	)
	const
{
	// build equations the way Mathematica wants them

	const JSize eqCount = lhs.GetElementCount();
	assert( eqCount == rhs.GetElementCount() );

	JString equations;
	for (JIndex i=1; i<=eqCount; i++)
		{
		equations += *(lhs.NthElement(i));  
		equations += " == ";
		equations += *(rhs.NthElement(i));
		if (i < eqCount)
			{
			equations += ", ";
			}
		}

	return PrivateSolveEqns(equations, variables, result);
}


JBoolean
JMathematicaSM::PrivateSolveEqns
	(
	const JString&				equations,
	const JPtrArray<JString>&	variables,
	JString*					result
	)
	const
{
	// build list of variables to solve for

	JString vars;
	const JSize varCount = variables.GetElementCount();
	for (JIndex i=1; i<=varCount; i++)
		{
		vars += *(variables.NthElement(i));
		if (i < varCount)
			{
			vars += ", ";
			}
		}

	// build the final command

	JString cmd = "Simplify[ Solve[ {";
	cmd += equations;
	cmd += "}, {";
	cmd += vars;
	cmd += "} ]]";

	// return the result as a reference %n

	JString outputName, tempResult;
	const JBoolean ok = SendRecvText(cmd, &outputName, &tempResult);
	if (ok)
		{
		*result = "%" + outputName;
		}
	else
		{
		result->Clear();
		}
	return ok;
}

/******************************************************************************
 ConvertToMathematica (private)

 ******************************************************************************/

JString
JMathematicaSM::ConvertToMathematica
	(
	const JFunction* f
	)
	const
{
	return f->PrintForMathematica();
}

/******************************************************************************
 ConvertFromMathematica (private)

	{} -> ()

 ******************************************************************************/

void
JMathematicaSM::ConvertFromMathematica
	(
	JString* str
	)
	const
{
	const JSize length = str->GetLength();
	for (JIndex i=1; i<=length; i++)
		{
		const JCharacter c = str->GetCharacter(i);
		if (c == '{')
			{
			str->SetCharacter(i, '(');
			}
		else if (c == '}')
			{
			str->SetCharacter(i, ')');
			}
		}
}

/******************************************************************************
 SendRecvText (private)

	Send the given command to Mathematica and return the result in a
	form that can later be fed back into Mathematica.

	InputForm[ command ]

 ******************************************************************************/

JBoolean
JMathematicaSM::SendRecvText
	(
	const JCharacter*	command,
	JString*			outputName,
	JString*			result
	)
	const
{
//	cout << endl << command << endl;

	// clean out any junk in the queue

	FlushInput();

	// issue the command

	JString cmd = "InputForm[ ";
	cmd += command;
	cmd += " ]";

	SendText(cmd);
	return ReceiveText(outputName, result);
}

/******************************************************************************
 SendText (private)

	Send the given command to Mathematica.

 ******************************************************************************/

void
JMathematicaSM::SendText
	(
	const JCharacter* command
	)
	const
{
#ifdef HAS_MATHLINK

	MLPutFunction(itsLink, "EnterTextPacket", 1);
	MLPutString(itsLink, const_cast<char*>(command));
	MLEndPacket(itsLink);

#endif
}

/******************************************************************************
 ReceiveText (private)

	Receive a text string from Mathematica.

	*** Don't call this unless you expect a result.

 ******************************************************************************/

JBoolean
JMathematicaSM::ReceiveText
	(
	JString* outputName,
	JString* result
	)
	const
{
#ifndef HAS_MATHLINK

	*outputName = "1";
	*result = "%";
	return kJTrue;		// for testing without mathlink

#else

	JProgressDisplay* pg = CreatePG();

	// ignore all packets until output name is received

	*outputName = "0";
	result->Clear();

	if (!WaitForPacket(OUTPUTNAMEPKT, pg))
		{
		DeletePG(pg);
		return kJFalse;
		}

	// get the part containing the index

	const JCharacter* outputStr;
	MLGetString(itsLink, &outputStr);
	*outputName = outputStr;
	MLDisownString(itsLink, outputStr);

	JIndex openIndex, closeIndex;
	const JBoolean foundOpen  = outputName->LocateSubstring("[", &openIndex);
	const JBoolean foundClose = outputName->LocateSubstring("]", &closeIndex);
	assert( foundOpen && foundClose && openIndex+1 < closeIndex );
	*outputName = outputName->GetSubstring(openIndex+1, closeIndex-1);
	outputName->TrimWhitespace();

	// ignore all packets until text is received

	if (!WaitForPacket(RETURNTEXTPKT, pg))
		{
		DeletePG(pg);
		return kJFalse;
		}

	// return the answer as a JString

	const JCharacter* resultStr;
	MLGetString(itsLink, &resultStr);
	*result = resultStr;
	MLDisownString(itsLink, resultStr);

	// remove the newline characters

	JIndex newlineIndex;
	while (result->LocateSubstring("\n", &newlineIndex))
		{
		result->SetCharacter(newlineIndex, ' ');
		}
	while (result->LocateSubstring("\\012", &newlineIndex))
		{
		result->ReplaceSubstring(newlineIndex, newlineIndex+3, " ");
		}

	DeletePG(pg);
	return kJTrue;

#endif
}

/******************************************************************************
 WaitForPacket (private)

	Toss packets until we get one of the specified type.  We check the packets
	that we toss and print the useful ones to a JProgressDisplay so the user
	can decide if they mean anything.  We also allow the user to cancel the
	operation by trapping the interrupt signal.

	Returns kJTrue if successful.

	*** Don't call this unless you expect a result.

 ******************************************************************************/

JBoolean
JMathematicaSM::WaitForPacket
	(
	const int			packetType,
	JProgressDisplay*	pg
	)
	const
{
#ifndef HAS_MATHLINK

	// This shouldn't be called unless we have mathlink.

	return kJFalse;

#else

	JBoolean ok = kJTrue;
	while (ok)
		{
		JWait(1.0);
		ok = pg->IncrementProgress();
		if (!ok)
			{
			break;
			}

		MLFlush(itsLink);
		if (MLReady(itsLink))
			{
			const int type = MLNextPacket(itsLink);
			if (type == packetType)
				{
				break;
				}
			else if (type == TEXTPKT)
				{
				const JCharacter* s;
				MLGetString(itsLink, &s);
				JString msg = s;
				MLDisownString(itsLink, s);
				JIndex i;
				while (msg.LocateSubstring("\\012", &i))
					{
					msg.ReplaceSubstring(i, i+3, "\n");
					}
				ok = pg->IncrementProgress(msg);
				}
			else if (type == SYNTAXPKT)
				{
				int pos;
				MLGetInteger(itsLink, &pos);
				JString str = pos;
				str.Prepend("Syntax error at position ");
				ok = pg->IncrementProgress(str);
				}
			else if (type == 0)
				{
//				MLClearError(itsLink);		// not in Mathematica 3.0 library
				}

			MLNewPacket(itsLink);
			}
		}

	return ok;

#endif
}

/******************************************************************************
 Progress display functions (private)

	These are useful since we insist that a pg be passed into WaitForPacket().
	(This in turns cuts the number of progress display windows in half.)

 ******************************************************************************/

JProgressDisplay*
JMathematicaSM::CreatePG()
	const
{
	JProgressDisplay* pg = JNewPG();
	pg->VariableLengthProcessBeginning("Waiting for Mathematica...", kJTrue, kJFalse);
	return pg;
}

void
JMathematicaSM::DeletePG
	(
	JProgressDisplay* pg
	)
	const
{
	pg->ProcessFinished();
	delete pg;
}

/******************************************************************************
 FlushInput (private)

	Toss all packets that are already in the input queue.

 ******************************************************************************/

void
JMathematicaSM::FlushInput()
	const
{
#ifdef HAS_MATHLINK

	while (MLReady(itsLink))
		{
		MLNextPacket(itsLink);
		MLNewPacket(itsLink);
		}

#endif
}
