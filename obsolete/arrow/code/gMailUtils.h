/******************************************************************************
 gMailUtils.h

	Copyright (C) 1997 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_gMailUtils
#define _H_gMailUtils

#include <GMessageHeader.h>
#include <JPtrArray-JString.h>

class GMAttachmentTable;
class GMessageHeaderList;
class JTextEditor;

void GParseNameLists(const JString& to, const JString& cc, const JString& bcc,
					JPtrArray<JString>& tos, JPtrArray<JString>& ccs,
					JPtrArray<JString>& bccs);

void GParseNameLists(const JString& to, const JString& cc, const JString& bcc,
					JPtrArray<JString>& names);

void GParseNameList(const JString& list,
					JPtrArray<JString>& names,
					const JBoolean forName = kJFalse);
void GGetRealNames(const JString& list,	JPtrArray<JString>& names);

JBoolean GVerifyPGPNames(const JPtrArray<JString>& names);
JBoolean GVerifyGPGNames(const JPtrArray<JString>& names);

JString GGetAddress(const JString& fullname);
JString GGetName(const JString& fullname);
void	GSplitAddressAndName(const JString& fullname, JString* address, JString* name);
void	GFixHeaderForReply(GMessageHeader::ReplyStatus status, JString* sub);
void	GCompressWhitespace(JString* sub);

void	GReadQuote(std::istream& is, JString* str);
void	GReadComment(std::istream& is, JString* str);
void	GReadBracket(std::istream& is, JString* str);
void	GSaveMessage (std::istream& is, std::ostream& os, GMessageHeader* header,
					 const JBoolean update = kJFalse,
					 const JBoolean flush = kJTrue);
JBoolean	GAppendMessage(const JCharacter* srcMBox, const JCharacter* destMBox,
						  GMessageHeaderList* list);

JString		GGetRelativeToHome(const JString& filename);
JBoolean	GGetAbsoluteFromHome(const JString& filename, JString* file);

// Encryption utilities

JBoolean GEncryptPGP2_6_2(const JString& text, const JString& passwd, JPtrArray<JString>* names, JString* newText);
JBoolean GEncryptGPG1_0(const JString& text, JPtrArray<JString>* names, JString* newText);

JBoolean GDecryptPGP2_6_2(const JString& text, const JString& passwd, JString* newText);
JBoolean GDecryptGPG1_0(const JString& text, const JString& passwd, JString* newText);

// MIME utilities

void	GMGetMIMESource(GMAttachmentTable* table, JTextEditor* editor,
						JString* body, JString* header);

JString GMMD5Encrypt(const JCharacter* text);
#endif
