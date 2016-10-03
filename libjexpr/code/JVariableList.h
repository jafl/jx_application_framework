/******************************************************************************
 JVariableList.h

	Interface for JVariableList class.

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JVariableList
#define _H_JVariableList

#include <JContainer.h>
#include <JPtrArray.h>
#include <JComplex.h>

class JString;
class JDecision;
class JFunction;

class JVariableList : public JContainer
{
public:

	enum MatchResult
	{
		kNoMatch,
		kSingleMatch,
		kMultipleMatch
	};

public:

	JVariableList();
	JVariableList(const JVariableList& source);

	virtual ~JVariableList();

	virtual const JString&	GetVariableName(const JIndex index) const = 0;
	virtual void			GetVariableName(const JIndex index, JString* name,
											JString* subscript) const = 0;

	virtual JBoolean	IsNumeric(const JIndex index) const = 0;
	virtual JBoolean	IsDiscrete(const JIndex index) const = 0;

	virtual JBoolean	IsArray(const JIndex index) const = 0;
	virtual JBoolean	ArrayIndexValid(const JIndex variableIndex,
										const JIndex elementIndex) const = 0;

	virtual JBoolean		ValueIsKnown(const JIndex variableIndex,
										 const JIndex elementIndex) const = 0;
	JBoolean				ValueIsUnknown(const JIndex variableIndex,
										   const JIndex elementIndex) const;
	virtual const JString&	GetUnknownValueSymbol(const JIndex index) const = 0;

	virtual JBoolean	GetNumericValue(const JIndex variableIndex,
										const JIndex elementIndex,
										JFloat* value) const = 0;
	virtual JBoolean	GetNumericValue(const JIndex variableIndex,
										const JIndex elementIndex,
										JComplex* value) const = 0;

	virtual void	SetNumericValue(const JIndex variableIndex,
									const JIndex elementIndex,
									const JFloat value) = 0;
	virtual void	SetNumericValue(const JIndex variableIndex,
									const JIndex elementIndex,
									const JComplex& value) = 0;

	virtual JIndex			GetDiscreteValue(const JIndex variableIndex,
											 const JIndex elementIndex) const = 0;
	virtual JSize			GetDiscreteValueCount(const JIndex index) const = 0;
	virtual const JString&	GetDiscreteValueName(const JIndex variableIndex,
												 const JIndex valueIndex) const = 0;

	virtual void	SetDiscreteValue(const JIndex variableIndex,
									 const JIndex elementIndex,
									 const JIndex valueIndex) = 0;

	JBoolean	ParseVariableName(const JCharacter* expr, const JSize exprLength,
								  JIndex* index) const;
	MatchResult	FindUniqueVarName(const JCharacter* prefix, JIndex* index,
								  JString* maxPrefix) const;
	JBoolean	ParseDiscreteValue(const JCharacter* expr, const JSize exprLength,
								   const JIndex& variableIndex,
								   JIndex* valueIndex) const;

	JBoolean	HaveSameValues(const JIndex index1, const JIndex index2) const;

	JBoolean	OKToRemoveVariable(const JIndex variableIndex) const;

	// reference counting

	void	VariableUserCreated(JDecision* d) const;
	void	VariableUserDeleted(JDecision* d) const;

	void	VariableUserCreated(JFunction* f) const;
	void	VariableUserDeleted(JFunction* f) const;

protected:

	void		PushOnEvalStack(const JIndex variableIndex) const;
	void		PopOffEvalStack(const JIndex variableIndex) const;
	JBoolean	IsOnEvalStack(const JIndex variableIndex) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JPtrArray<JDecision>*	itsDVarUserList;	// doesn't own objects
	JPtrArray<JFunction>*	itsFVarUserList;	// doesn't own objects
	JArray<JBoolean>*		itsEvalStack;

private:

	void	JVariableListX();
	void	VariablesInserted(const JOrderedSetT::ElementsInserted& info) const;
	void	VariablesRemoved(const JOrderedSetT::ElementsRemoved& info) const;
	void	VariableMoved(const JOrderedSetT::ElementMoved& info) const;
	void	VariablesSwapped(const JOrderedSetT::ElementsSwapped& info) const;

	// not allowed

	const JVariableList& operator=(const JVariableList& source);

public:

	// JBroadcaster messages

	static const JCharacter* kVarInserted;
	static const JCharacter* kVarRemoved;
	static const JCharacter* kVarMoved;
	static const JCharacter* kVarNameChanged;
	static const JCharacter* kVarValueChanged;
	static const JCharacter* kDiscValueNameChanged;

private:

	// base class for JBroadcaster messages

	class VarMessage : public JBroadcaster::Message
		{
		public:

			VarMessage(const JCharacter* type, const JIndex index)
				:
				JBroadcaster::Message(type),
				itsVarIndex(index)
				{ };

			JIndex
			GetIndex() const
			{
				return itsVarIndex;
			};

		private:

			JIndex	itsVarIndex;
		};

public:

	// new variable inserted

	class VarInserted : public VarMessage
		{
		public:

			VarInserted(const JIndex index)
				:
				VarMessage(kVarInserted, index)
				{ };

			void	AdjustIndex(JIndex* index) const;
		};

	// existing variable removed

	class VarRemoved : public VarMessage
		{
		public:

			VarRemoved(const JIndex index)
				:
				VarMessage(kVarRemoved, index)
				{ };

			JBoolean	AdjustIndex(JIndex* index) const;
		};

	// existing variable moved

	class VarMoved : public JBroadcaster::Message
		{
		public:

			VarMoved(const JIndex origIndex, const JIndex newIndex)
				:
				JBroadcaster::Message(kVarMoved),
				itsOrigIndex(origIndex),
				itsNewIndex(newIndex)
				{ };

			void	AdjustIndex(JIndex* index) const;

			JIndex
			GetOrigIndex() const
			{
				return itsOrigIndex;
			};

			JIndex
			GetNewIndex() const
			{
				return itsNewIndex;
			};

		private:

			JIndex	itsOrigIndex;
			JIndex	itsNewIndex;
		};

	// variable name changed

	class VarNameChanged : public JBroadcaster::Message
		{
		public:

			VarNameChanged(const JIndex index)
				:
				JBroadcaster::Message(kVarNameChanged),
				itsVarIndex(index)
				{ };

			JIndex
			GetIndex() const
			{
				return itsVarIndex;
			};

		private:

			JIndex	itsVarIndex;
		};

	// value of variable changed

	class VarValueChanged : public JBroadcaster::Message
		{
		public:

			VarValueChanged(const JIndex varIndex, const JIndex elementIndex)
				:
				JBroadcaster::Message(kVarValueChanged),
				itsVarIndex(varIndex),
				itsElementIndex(elementIndex)
				{ };

			JIndex
			GetVarIndex() const
			{
				return itsVarIndex;
			};

			JIndex
			GetElementIndex() const
			{
				return itsElementIndex;
			};

		private:

			JIndex	itsVarIndex, itsElementIndex;
		};

	// name of discrete value for a particular discrete variable changed

	class DiscValueNameChanged : public JBroadcaster::Message
		{
		public:

			DiscValueNameChanged(const JIndex varIndex, const JIndex valueIndex)
				:
				JBroadcaster::Message(kDiscValueNameChanged),
				itsVarIndex(varIndex),
				itsValueIndex(valueIndex)
				{ };

			JIndex
			GetVarIndex() const
			{
				return itsVarIndex;
			};

			JIndex
			GetValueIndex() const
			{
				return itsValueIndex;
			};

		private:

			JIndex	itsVarIndex, itsValueIndex;
		};
};

/******************************************************************************
 ValueIsUnknown

 ******************************************************************************/

inline JBoolean
JVariableList::ValueIsUnknown
	(
	const JIndex variableIndex,
	const JIndex elementIndex
	)
	const
{
	return !ValueIsKnown(variableIndex, elementIndex);
}

/******************************************************************************
 IsOnEvalStack (protected)

 ******************************************************************************/

inline JBoolean
JVariableList::IsOnEvalStack
	(
	const JIndex variableIndex
	)
	const
{
	return itsEvalStack->GetElement(variableIndex);
}

#endif
