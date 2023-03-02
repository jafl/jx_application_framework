/******************************************************************************
 JXSpellList.h

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JXSpellList
#define _H_JXSpellList

#include "JXStringList.h"

class JXSpellList : public JXStringList
{

public:

	JXSpellList(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	~JXSpellList() override;

protected:

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

private:

	class SpellBase : public JBroadcaster::Message
		{
		public:

			SpellBase(const JUtf8Byte* type, const JString& word)
				:
				JBroadcaster::Message(type),
				itsWord(word)
				{ };

			const JString&
			GetWord() const
				{
				return itsWord;
				};

		private:

			const JString& itsWord;
		};

public:

	// JBroadcaster messages

	static const JUtf8Byte* kWordSelected;
	static const JUtf8Byte* kReplaceWord;
	static const JUtf8Byte* kReplaceWordAll;

	class WordSelected : public SpellBase
		{
		public:

			WordSelected(const JString& word)
				:
				SpellBase(kWordSelected, word)
				{ };
		};

	class ReplaceWord : public SpellBase
		{
		public:

			ReplaceWord(const JString& word)
				:
				SpellBase(kReplaceWord, word)
				{ };
		};

	class ReplaceWordAll : public SpellBase
		{
		public:

			ReplaceWordAll(const JString& word)
				:
				SpellBase(kReplaceWordAll, word)
				{ };
		};
};

#endif
