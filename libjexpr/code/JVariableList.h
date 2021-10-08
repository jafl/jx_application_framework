/******************************************************************************
 JVariableList.h

	Interface for JVariableList class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVariableList
#define _H_JVariableList

#include <jx-af/jcore/JContainer.h>
#include <jx-af/jcore/JPtrArray.h>
#include <jx-af/jcore/JComplex.h>

class JString;
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

	~JVariableList() override;

	virtual const JString&	GetVariableName(const JIndex index) const = 0;
	virtual void			GetVariableName(const JIndex index, JString* name,
											JString* subscript) const = 0;

	virtual bool	IsArray(const JIndex index) const = 0;
	virtual bool	ArrayIndexValid(const JIndex variableIndex,
									const JIndex elementIndex) const = 0;

	virtual bool	GetNumericValue(const JIndex variableIndex,
									const JIndex elementIndex,
									JFloat* value) const = 0;
	virtual bool	GetNumericValue(const JIndex variableIndex,
									const JIndex elementIndex,
									JComplex* value) const = 0;

	virtual void	SetNumericValue(const JIndex variableIndex,
									const JIndex elementIndex,
									const JFloat value) = 0;
	virtual void	SetNumericValue(const JIndex variableIndex,
									const JIndex elementIndex,
									const JComplex& value) = 0;

	bool	ParseVariableName(const JString& name, JIndex* index) const;
	bool	OKToRemoveVariable(const JIndex variableIndex) const;

	static bool	NameValid(const JString& name);

	// reference counting

	void	VariableUserCreated(JFunction* f) const;
	void	VariableUserDeleted(JFunction* f) const;

protected:

	void	PushOnEvalStack(const JIndex variableIndex) const;
	void	PopOffEvalStack(const JIndex variableIndex) const;
	bool	IsOnEvalStack(const JIndex variableIndex) const;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JPtrArray<JFunction>*	itsVarUserList;	// doesn't own objects
	JArray<bool>*			itsEvalStack;

private:

	void	JVariableListX();
	void	VariablesInserted(const JListT::ElementsInserted& info) const;
	void	VariablesRemoved(const JListT::ElementsRemoved& info) const;
	void	VariableMoved(const JListT::ElementMoved& info) const;
	void	VariablesSwapped(const JListT::ElementsSwapped& info) const;

	// not allowed

	JVariableList& operator=(const JVariableList&) = delete;

public:

	// JBroadcaster messages

	static const JUtf8Byte* kVarInserted;
	static const JUtf8Byte* kVarRemoved;
	static const JUtf8Byte* kVarMoved;
	static const JUtf8Byte* kVarNameChanged;
	static const JUtf8Byte* kVarValueChanged;
	static const JUtf8Byte* kDiscValueNameChanged;

private:

	// base class for JBroadcaster messages

	class VarMessage : public JBroadcaster::Message
		{
		public:

			VarMessage(const JUtf8Byte* type, const JIndex index)
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

			bool	AdjustIndex(JIndex* index) const;
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
 IsOnEvalStack (protected)

 ******************************************************************************/

inline bool
JVariableList::IsOnEvalStack
	(
	const JIndex variableIndex
	)
	const
{
	return itsEvalStack->GetElement(variableIndex);
}

#endif
