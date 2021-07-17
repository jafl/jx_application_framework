/******************************************************************************
 GLDLFitModule.h

	Interface for the GLDLFitModule class

	Copyright (C) 2000 by Glenn W. Bach.
	
 *****************************************************************************/

#ifndef _H_GLDLFitModule
#define _H_GLDLFitModule

#include <jTypes.h>
#include <JPtrArray-JString.h>

typedef JFloat (EvalFn)(const JFloat*, const JFloat);
typedef JFloat (InitialValFn)(const JSize, const JFloat*, const JFloat*, const JFloat*, const JFloat*, JFloat*);

class ACE_DLL;
class JVector;

class GLDLFitModule
{
public:

	static bool Create(const JString& moduleName, GLDLFitModule** fit);
	virtual ~GLDLFitModule();

	bool	HasStartValues() const;
	bool	GetStartValues(const JArray<JFloat>* x, const JArray<JFloat>* y, 
							   const JArray<JFloat>* xerr, const JArray<JFloat>* yerr, 
							   JVector* p);
	JFloat		Function(const JFloat x);
	JFloat		FPrimed(const JFloat x);
	bool	HasFPrimed() const;
	void		SetCurrentParameters(const JVector& p);

	JSize			GetParameterCount() const;
	const JString& 	GetParameterName(const JIndex index) const;

	const JString& 	GetFunctionalForm() const;
	const JString& 	GetFitName() const;

protected:

	GLDLFitModule(ACE_DLL* module, EvalFn* function, EvalFn* fprimed, 
				  InitialValFn* initFn,
				  const JSize count, const JUtf8Byte** parms, 
				  const JUtf8Byte* form, const JUtf8Byte* name);

private:

	JString				itsFunctionalForm;
	JString				itsFitName;
	JSize				itsParmCount;
	JPtrArray<JString>*	itsParmNames;
	JVector*			itsParameters;

	EvalFn*			itsFunction;
	EvalFn*			itsFPrimed;
	InitialValFn*	itsGetStartValFn;

	ACE_DLL*	itsModule;	
	
//	JFloat		(*itsEvaluateFn)(const JFloat*, const JFloat);
//	JFloat		(*itsEvaluateDerivativeFn)(const JFloat*, const JFloat);
	

private:

	// not allowed

	GLDLFitModule(const GLDLFitModule& source);
	const GLDLFitModule& operator=(const GLDLFitModule& source);

public:
};

#endif
