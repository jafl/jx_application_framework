
/******************************************************************************
 PrivateCleanRightMargin (private)

	*newText is the cleaned up version of the paragraph containing the caret
	or start of selection.

	*origTextRange contains the range that was cleaned up, excluding the last
	newline.

	*newCaretIndex is the index required to maintain the position of the caret.

	Returns kJFalse if the caret was not in a paragraph.

 ******************************************************************************/

JBoolean
JTextEditor::PrivateCleanRightMargin
	(
	const JBoolean		coerce,
	JIndexRange*		origTextRange,
	JString*			newText,
	JRunArray<JFont>*	newStyles,
	JIndex*				newCaretIndex
	)
	const
{
	origTextRange->SetToNothing();
	newText->Clear();
	newStyles->RemoveAll();
	*newCaretIndex = 0;

	if (itsBuffer->IsEmpty())
		{
		return kJFalse;
		}

	const JIndex caretChar = GetInsertionIndex();
	if (caretChar == itsBuffer->GetLength()+1 && EndsWithNewline())
		{
		return kJFalse;
		}

	JIndex charIndex, ruleIndex;
	JString firstLinePrefix, restLinePrefix;
	JSize firstPrefixLength, restPrefixLength;
	if (!CRMGetRange(caretChar, coerce, origTextRange, &charIndex,
					 &firstLinePrefix, &firstPrefixLength,
					 &restLinePrefix, &restPrefixLength, &ruleIndex))
		{
		return kJFalse;
		}

	if (caretChar <= charIndex)
		{
		*newCaretIndex = caretChar;
		}

	// read in each word, convert it, write it out

	JSize currentLineWidth = 0;
	JBoolean requireSpace  = kJFalse;

	JString wordBuffer, spaceBuffer;
	JRunArray<JFont> wordStyles;
	while (charIndex <= origTextRange->last)
		{
		JSize spaceCount;
		JIndex rnwCaretIndex = 0;
		const CRMStatus status =
			CRMReadNextWord(&charIndex, origTextRange->last,
							&spaceBuffer, &spaceCount, &wordBuffer, &wordStyles,
							currentLineWidth, caretChar, &rnwCaretIndex,
							*newText, requireSpace);
		requireSpace = kJTrue;

		if (status == kFinished)
			{
			assert( charIndex == origTextRange->last+1 );
			break;
			}
		else if (status == kFoundWord)
			{
			if (newText->IsEmpty())
				{
				CRMAppendWord(newText, newStyles, &currentLineWidth, &rnwCaretIndex,
							  spaceBuffer, spaceCount, wordBuffer, wordStyles,
							  firstLinePrefix, firstPrefixLength);
				}
			else
				{
				CRMAppendWord(newText, newStyles, &currentLineWidth, &rnwCaretIndex,
							  spaceBuffer, spaceCount, wordBuffer, wordStyles,
							  restLinePrefix, restPrefixLength);
				}

			if (rnwCaretIndex > 0)
				{
				*newCaretIndex = origTextRange->first + rnwCaretIndex - 1;
				}
			}
		else
			{
			assert( status == kFoundNewLine );

			CRMTossLinePrefix(&charIndex, origTextRange->last, ruleIndex);

			// CRMGetRange() ensures this is strictly *inside* the text,
			// so caretChar == charIndex will be caught elsewhere.

			if (*newCaretIndex == 0 && caretChar < charIndex)
				{
				*newCaretIndex = origTextRange->first + newText->GetLength();
				}
			}
		}

	if (caretChar == origTextRange->last+1)
		{
		*newCaretIndex = origTextRange->first + newText->GetLength();
		}

	assert( *newCaretIndex != 0 );
	assert( newText->GetLength() == newStyles->GetElementCount() );

	return kJTrue;
}

/*******************************************************************************
 CRMGetRange (private)

	Returns the range of characters to reformat.
	Returns kJFalse if the caret is not in a paragraph.

	caretChar is the current location of the caret.

	If coerce, we search forward and backward from the caret location
	for blank lines and include all the text between these blank lines.
	Blank lines are defined as lines that contain nothing but prefix characters.

	Otherwise, we first search backward until we find a blank line or a
	line prefix that can't generate the following one.  Then we search
	forward until we find a blank line or the prefix changes.

	The latter method is safer because it doesn't change the prefix of any of
	the lines.  The former is useful for forcing a change in the prefix.

 ******************************************************************************/

JBoolean
JTextEditor::CRMGetRange
	(
	const JIndex	caretChar,
	const JBoolean	coerce,
	JIndexRange*	range,
	JIndex*			textStartIndex,
	JString*		firstLinePrefix,
	JSize*			firstPrefixLength,
	JString*		restLinePrefix,
	JSize*			restPrefixLength,
	JIndex*			returnRuleIndex
	)
	const
{
	range->Set(GetParagraphStart(caretChar), GetParagraphEnd(caretChar));
	while (range->last > 0 && itsBuffer->GetCharacter(range->last) == '\n')
		{
		range->last--;
		}

	// If the line we are on is empty, quit immediately.

	JIndex tempStart = range->first;
	JString origLinePrefix;
	JSize prefixLength;
	JIndex ruleIndex = 0;
	if (range->IsEmpty() ||
		!CRMGetPrefix(&tempStart, range->last,
					  &origLinePrefix, &prefixLength, &ruleIndex) ||
		CRMLineMatchesRest(*range))
		{
		return kJFalse;
		}

	// search backward for a blank line or a change in the prefix (if !coerce)
	// (If range->first==2, the line above us is blank.)

	JString currLinePrefix, nextLinePrefix = origLinePrefix;
	while (range->first > 2)
		{
		const JIndex newStart = GetParagraphStart(range->first - 1);
		tempStart             = newStart;
		ruleIndex             = 0;
		if (tempStart >= range->first - 1 ||
			!CRMGetPrefix(&tempStart, range->first - 2,
						  &currLinePrefix, &prefixLength, &ruleIndex) ||
			CRMLineMatchesRest(JIndexRange(newStart, range->first - 2)) ||
			(!coerce &&
			 CRMBuildRestPrefix(currLinePrefix, ruleIndex, &prefixLength) != nextLinePrefix))
			{
			break;
			}
		range->first   = newStart;
		nextLinePrefix = currLinePrefix;
		ruleIndex      = 0;
		}

	// search forward for a blank line or a change in the prefix (if !coerce)
	// (If range->last==bufLength-1, the text ends with a newline.)

	*textStartIndex  = range->first;
	*returnRuleIndex = 0;
	const JBoolean hasText =
		CRMGetPrefix(textStartIndex, range->last,
					 firstLinePrefix, firstPrefixLength, returnRuleIndex);
	assert( hasText );

	*restLinePrefix = CRMBuildRestPrefix(*firstLinePrefix, *returnRuleIndex,
										 restPrefixLength);

	while (range->last < itsBuffer->GetLength()-1)
		{
		tempStart     = range->last + 2;
		JIndex newEnd = GetParagraphEnd(tempStart);
		if (itsBuffer->GetCharacter(newEnd) == '\n')	// could hit end of text instead
			{
			newEnd--;
			}

		JIndex tempRuleIndex = *returnRuleIndex;
		if (newEnd < tempStart ||
			!CRMGetPrefix(&tempStart, newEnd,
						  &currLinePrefix, &prefixLength, &tempRuleIndex) ||
			(!coerce && currLinePrefix != *restLinePrefix))
			{
			break;
			}
		range->last = newEnd;
		}

	return kJTrue;
}

/*******************************************************************************
 CRMGetPrefix (private)

	Returns the prefix to be used for each line and updates *startChar to point
	to the first character after the prefix.

	*prefixLength is set to the length of *linePrefix in characters.  This
	can be greater than linePrefix->GetLength() because of tabs.

	Returns kJFalse if the entire range qualifies as a prefix.

 ******************************************************************************/

JBoolean
JTextEditor::CRMGetPrefix
	(
	JIndex*			startChar,
	const JIndex	endChar,
	JString*		linePrefix,
	JSize*			prefixLength,
	JIndex*			ruleIndex
	)
	const
{
	const JIndexRange prefixRange =
		CRMMatchPrefix(JIndexRange(*startChar, endChar), ruleIndex);

	*startChar    = prefixRange.last + 1;
	*linePrefix   = itsBuffer->GetSubstring(prefixRange);
	*prefixLength = CRMCalcPrefixLength(linePrefix);
	return JConvertToBoolean(*startChar <= endChar);
}

/*******************************************************************************
 CRMMatchPrefix (private)

	Returns the range of characters that qualifies as a prefix.

	To match a prefix, all CRMRule::first's and [ \t]* are tried.
	The longest match is used.

	If *ruleIndex > 0, CRMRule::rest for the specified rule is tried *first*,
	and if this matches, it preempts everything else.

	When the function returns, *ruleIndex is the CRMRule::first that matched.
	(It remains 0 if the default rule is used, since this doesn't have "rest".)

 ******************************************************************************/

static const JRegex defaultCRMPrefixRegex = "[ \t]*";

JIndexRange
JTextEditor::CRMMatchPrefix
	(
	const JIndexRange&	textRange,
	JIndex*				ruleIndex
	)
	const
{
	JIndexRange matchRange;

	if (itsCRMRuleList != NULL && *ruleIndex > 0)
		{
		const CRMRule rule = itsCRMRuleList->GetElement(*ruleIndex);
		if ((rule.rest)->MatchWithin(*itsBuffer, textRange, &matchRange) &&
			matchRange.first == textRange.first)
			{
			return matchRange;
			}
		}

	JIndexRange range;
	if (itsCRMRuleList != NULL)
		{
		const JSize count = itsCRMRuleList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			const CRMRule rule = itsCRMRuleList->GetElement(i);
			if ((rule.first)->MatchWithin(*itsBuffer, textRange, &range) &&
				range.first == textRange.first &&
				range.last  >  matchRange.last)
				{
				matchRange = range;
				*ruleIndex = i;
				}
			}
		}

	// check equality of range::last in case prefix is empty

	const JBoolean defMatch =
		defaultCRMPrefixRegex.MatchWithin(*itsBuffer, textRange, &range);
	assert( defMatch && range.first == textRange.first );
	if (range.last >= matchRange.last || itsCRMRuleList == NULL)
		{
		matchRange = range;
		*ruleIndex = 0;
		}

	return matchRange;
}

/*******************************************************************************
 CRMLineMatchesRest (private)

	Returns kJTrue if the given range is matched by any "rest" pattern.
	Used at beginning of CRMGetRange as part of check if line is empty.

 ******************************************************************************/

JBoolean
JTextEditor::CRMLineMatchesRest
	(
	const JIndexRange& range
	)
	const
{
	if (itsCRMRuleList != NULL)
		{
		JIndexRange matchRange;
		const JSize count = itsCRMRuleList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			const CRMRule rule = itsCRMRuleList->GetElement(i);
			if ((rule.rest)->MatchWithin(*itsBuffer, range, &matchRange) &&
				range == matchRange)
				{
				return kJTrue;
				}
			}
		}

	return kJFalse;
}

/*******************************************************************************
 CRMCalcPrefixLength (private)

	Returns the length of *linePrefix in characters.  This can be greater
	than linePrefix->GetLength() because of tabs.  *linePrefix may be modified.

 ******************************************************************************/

JSize
JTextEditor::CRMCalcPrefixLength
	(
	JString* linePrefix
	)
	const
{
	if (linePrefix->IsEmpty())
		{
		return 0;
		}

	const JString s    = *linePrefix;
	JSize prefixLength = 0;
	linePrefix->Clear();

	const JSize length = s.GetLength();
	for (JIndex i=1; i<=length; i++)
		{
		const JCharacter c = s.GetCharacter(i);
		if (c == '\t')
			{
			CRMConvertTab(linePrefix, &prefixLength, 0);
			}
		else
			{
			linePrefix->AppendCharacter(c);
			prefixLength++;
			}
		}

	return prefixLength;
}

/*******************************************************************************
 CRMBuildRestPrefix (private)

	Returns the line prefix to be used for all lines after the first one.

 ******************************************************************************/

JString
JTextEditor::CRMBuildRestPrefix
	(
	const JString&	firstLinePrefix,
	const JIndex	ruleIndex,
	JSize*			prefixLength
	)
	const
{
	JString s = firstLinePrefix;
	if (itsCRMRuleList != NULL && ruleIndex > 0)
		{
		JArray<JIndexRange> matchList;
		const CRMRule rule   = itsCRMRuleList->GetElement(ruleIndex);
		const JBoolean match = (rule.first)->Match(s, &matchList);
		assert( match &&
				(matchList.GetFirstElement()).first == 1 &&
				(matchList.GetFirstElement()).last  == s.GetLength() );

		JIndexRange newRange;
		(rule.first)->Replace(&s, matchList, &newRange);
		}

	*prefixLength = CRMCalcPrefixLength(&s);
	return s;
}

/*******************************************************************************
 CRMTossLinePrefix (private)

	Increments *charIndex past whatever qualifies as a line prefix.

	The default is to toss all character approved by CRMIsPrefixChar().

 ******************************************************************************/

void
JTextEditor::CRMTossLinePrefix
	(
	JIndex*			charIndex,
	const JIndex	endChar,
	const JIndex	origRuleIndex
	)
	const
{
	JIndex ruleIndex = origRuleIndex;
	const JIndexRange prefixRange =
		CRMMatchPrefix(JIndexRange(*charIndex, endChar), &ruleIndex);
	*charIndex = prefixRange.last + 1;
}

/*******************************************************************************
 CRMConvertTab (virtual protected)

	Appends the tab to *charBuffer and increments *charCount appropriately.
	The default is to physically append the tab character and use
	CRMGetTabWidth() to increment *charCount.  Note that *charCount may be
	longer than charBuffer->GetLength().

	currentLineWidth is the number of characters on the line -excluding-
	what is in *charBuffer.

	Derived classes can override this to, for example, append some spaces
	instead of a tab character.  (How's that for splitting an infinitive?)

 ******************************************************************************/

void
JTextEditor::CRMConvertTab
	(
	JString*	charBuffer,
	JSize*		charCount,
	const JSize	currentLineWidth
	)
	const
{
	charBuffer->AppendCharacter('\t');
	*charCount += CRMGetTabWidth(currentLineWidth + *charCount);
}

/*******************************************************************************
 CRMGetTabWidth (virtual protected)

	Returns the number of spaces to which the tab is equivalent.
	The default is to round up to the nearest multiple of GetCRMTabCharCount().
	The default value for this is 8 since this is what all UNIX programs use.

	textColumn starts at zero at the left margin.

 ******************************************************************************/

JSize
JTextEditor::CRMGetTabWidth
	(
	const JIndex textColumn
	)
	const
{
	return itsCRMTabCharCount - (textColumn % itsCRMTabCharCount);
}

/*******************************************************************************
 CRMIsEOS (private)

 ******************************************************************************/

inline int
JTextEditor::CRMIsEOS
	(
	const JCharacter c
	)
	const
{
	return (c == '.' || c == '?' || c == '!');
}

/*******************************************************************************
 CRMReadNextWord (private)

	Read one block of { spaces + word (non-spaces) } from itsBuffer, starting
	at *charIndex, stopping if we get as far as endIndex.  *spaceBuffer
	contains the whitespace (spaces + tabs) that was found.  *spaceCount is
	the equivalent number of spaces.  *wordBuffer contains the word.
	*charIndex is incremented to point to the next character to read.

	Newlines are treated separately.  If a newline is encountered while reading
	spaces, we throw out the spaces and return kFoundNewLine.  If a newline
	is encountered while reading a word, we leave it for the next time, when
	we immediately return kFoundNewLine.

	When we pass the position origCaretIndex, we set *newCaretIndex to be
	the index into *spaceBuffer+*wordBuffer.  Otherwise, we do not change
	*newCaretIndex.

 ******************************************************************************/

JTextEditor::CRMStatus
JTextEditor::CRMReadNextWord
	(
	JIndex*				charIndex,
	const JIndex		endIndex,
	JString*			spaceBuffer,
	JSize*				spaceCount,
	JString*			wordBuffer,
	JRunArray<JFont>*	wordStyles,
	const JSize			currentLineWidth,
	const JIndex		origCaretIndex,
	JIndex*				newCaretIndex,
	const JString&		newText,
	const JBoolean		requireSpace
	)
	const
{
	// read the whitespace

	spaceBuffer->Clear();
	*spaceCount = 0;

	while (*charIndex <= endIndex)
		{
		if (*charIndex == origCaretIndex)
			{
			*newCaretIndex = spaceBuffer->GetLength() + 1;
			}

		const JCharacter c = itsBuffer->GetCharacter(*charIndex);
		if (c == ' ')
			{
			(*charIndex)++;
			spaceBuffer->AppendCharacter(c);
			(*spaceCount)++;
			}
		else if (c == '\t')
			{
			(*charIndex)++;
			CRMConvertTab(spaceBuffer, spaceCount, currentLineWidth);
			}
		else if (c == '\n')			// we can ignore the spaces
			{
			(*charIndex)++;
			spaceBuffer->Clear();
			*spaceCount = 0;
			return kFoundNewLine;
			}
		else						// found beginning of word
			{
			break;
			}
		}

	if (*charIndex == endIndex+1)	// we can ignore the spaces
		{
		return kFinished;
		}

	// read the word

	wordBuffer->Clear();

	const JIndex wordStart = *charIndex;
	while (*charIndex <= endIndex)
		{
		if (*charIndex == origCaretIndex)
			{
			*newCaretIndex = spaceBuffer->GetLength() + wordBuffer->GetLength() + 1;
			}

		const JCharacter c = itsBuffer->GetCharacter(*charIndex);
		if (c == ' ' || c == '\t' || c == '\n')
			{
			break;
			}

		wordBuffer->AppendCharacter(c);
		(*charIndex)++;
		}

	wordStyles->RemoveAll();
	wordStyles->InsertElementsAtIndex(1, *itsStyles, wordStart, wordBuffer->GetLength());

	// After a newline, the whitespace may have been tossed
	// as belonging to the prefix, but we still need some space.
	//
	// Punctuation is assumed to be the end of a sentence if the
	// following word does not start with a lower case letter.

	if (*spaceCount == 0 && requireSpace)
		{
		JCharacter c1 = newText.GetLastCharacter();
		JCharacter c2 = '\0';
		if (newText.GetLength() > 1)
			{
			c2 = newText.GetCharacter(newText.GetLength()-1);
			}
		if ((CRMIsEOS(c1) || (CRMIsEOS(c2) && c1 =='\"')) &&
			!JIsLower(wordBuffer->GetFirstCharacter()))
			{
			*spaceBuffer = "  ";
			}
		else
			{
			*spaceBuffer = " ";
			}

		*spaceCount = spaceBuffer->GetLength();
		if (*newCaretIndex > 0)
			{
			*newCaretIndex += *spaceCount;
			}
		}

	return kFoundWord;
}

/*******************************************************************************
 CRMAppendWord (private)

	Add the spaces and word to new text, maintaining the required line width.

	If *newCaretIndex>0, we convert it from an index in spaceBuffer+wordBuffer
	to an index in newText.

 ******************************************************************************/

void
JTextEditor::CRMAppendWord
	(
	JString*				newText,
	JRunArray<JFont>*		newStyles,
	JSize*					currentLineWidth,
	JIndex*					newCaretIndex,
	const JString&			spaceBuffer,
	const JSize				spaceCount,
	const JString&			wordBuffer,
	const JRunArray<JFont>&	wordStyles,
	const JString&			linePrefix,
	const JSize				prefixLength
	)
	const
{
	const JSize newLineWidth = *currentLineWidth + spaceCount + wordBuffer.GetLength();
	if (*currentLineWidth == 0 || newLineWidth > itsCRMLineWidth)
		{
		// calculate prefix font

		JFont prefixFont = wordStyles.GetFirstElement();
		prefixFont.ClearStyle();

		// terminate previous line

		if (!newText->IsEmpty())
			{
			newText->AppendCharacter('\n');
			newStyles->AppendElement(prefixFont);
			}
		if (!linePrefix.IsEmpty())
			{
			*newText += linePrefix;
			newStyles->AppendElements(prefixFont, linePrefix.GetLength());
			}

		// write word

		if (*newCaretIndex > 0)
			{
			if (*newCaretIndex <= spaceCount)
				{
				*newCaretIndex = 1;				// in spaces that we toss
				}
			else
				{
				*newCaretIndex -= spaceCount;	// in word
				}

			*newCaretIndex += newText->GetLength();
			}

		*newText += wordBuffer;
		newStyles->InsertElementsAtIndex(newStyles->GetElementCount()+1,
										 wordStyles, 1, wordStyles.GetElementCount());
		*currentLineWidth = prefixLength + wordBuffer.GetLength();
		}
	else	// newLineWidth <= itsCRMLineWidth
		{
		// append spaces + word at end of line

		if (*newCaretIndex > 0)
			{
			*newCaretIndex += newText->GetLength();
			}

		*newText += spaceBuffer;
		newStyles->AppendElements(JCalcWSFont(newStyles->GetLastElement(),
											  wordStyles.GetFirstElement()),
								  spaceBuffer.GetLength());
		*newText += wordBuffer;
		newStyles->InsertElementsAtIndex(newStyles->GetElementCount()+1,
										 wordStyles, 1, wordStyles.GetElementCount());

		if (newLineWidth < itsCRMLineWidth)
			{
			*currentLineWidth = newLineWidth;
			}
		else	// newLineWidth == itsCRMLineWidth
			{
			*currentLineWidth = 0;
			}
		}
}
