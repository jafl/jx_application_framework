%{
#define _H_GMScannerL

#include "GMScanner.h"
#include <stdio.h>
#include <jAssert.h>
%}

%option nodefault 8bit stack c++ full align
%option yyclass = "GMScanner" prefix="GM"
%option noyywrap

%x SUBJECT_STATE HEADER_STATE TOSS_STATE FROM_STATE DATE_STATE CC_STATE REPTO_STATE STATUS_STATE TO_STATE MIME_STATE CONTENT_TYPE_STATE CONTENT_DISPOSITION_STATE CONTENT_ENCODING_STATE REPLY_STATE PRIORITY_STATE ACCOUNT_STATE X_REPLY_STATE RECEIVED_STATE

FROMLINE ^From[[:space:]]+.*.{3}[[:space:]]+.{3}[[:space:]]+[[:digit:]]+[[:space:]]+([[:digit:]]+:)+[[:digit:]]+[[:space:]]+(...[[:space:]])?[[:digit:]]{4}

%%

{FROMLINE} {
	BEGIN(HEADER_STATE);
	itsCurrentHeaderStart = itsCurrentPosition;
	itsCurrentPosition += YYLeng();
	return kHeaderStart;
}

.|\n itsCurrentPosition += YYLeng();

<HEADER_STATE>{

^(Subject|SUBJECT): {
	BEGIN(SUBJECT_STATE);
	itsCurrentPosition += YYLeng();
	}

^(From|FROM): {
	BEGIN(FROM_STATE);
	itsCurrentPosition += YYLeng();
	}

^(To|TO): {
	BEGIN(TO_STATE);
	itsCurrentPosition += YYLeng();
	}

^(Date|DATE): {
	BEGIN(DATE_STATE);
	itsCurrentPosition += YYLeng();
	}

^[C|c][C|c]: {
	BEGIN(CC_STATE);
	itsCurrentPosition += YYLeng();
	}

^Reply-To: {
	BEGIN(REPTO_STATE);
	itsCurrentPosition += YYLeng();
	}

^Status: {
	BEGIN(STATUS_STATE);
	itsCurrentPosition += YYLeng();
	}

^(MIME|Mime)-[V|v]ersion: {
	BEGIN(MIME_STATE);
	itsCurrentPosition += YYLeng();
	}

^Content-[T|t]ype: {
	BEGIN(CONTENT_TYPE_STATE);
	itsCurrentPosition += YYLeng();
	}

^Content-[D|d]isposition: {
	BEGIN(CONTENT_DISPOSITION_STATE);
	itsCurrentPosition += YYLeng();
	}

^Content-[T|t]ransfer-[E|e]ncoding: {
	BEGIN(CONTENT_ENCODING_STATE);
	itsCurrentPosition += YYLeng();
	}

^X-Arrow-Reply-Status: {
	BEGIN(REPLY_STATE);
	itsCurrentPosition += YYLeng();
	}

^X-Arrow-Priority: {
	BEGIN(PRIORITY_STATE);
	itsCurrentPosition += YYLeng();
	}

^X-Arrow-Account: {
	BEGIN(ACCOUNT_STATE);
	itsCurrentPosition += YYLeng();
	}

^X-Status: {
	BEGIN(X_REPLY_STATE);
	itsCurrentPosition += YYLeng();
	}

^Received: {
	BEGIN(RECEIVED_STATE);
	itsCurrentPosition += YYLeng();
	}

^[^:\n]*: {
	BEGIN(TOSS_STATE);
	itsCurrentPosition += YYLeng();
	}

^\n {
	BEGIN(INITIAL);
	itsCurrentPosition += YYLeng();
	return kHeaderEnd;
	}

.|\n itsCurrentPosition += YYLeng();

}

<SUBJECT_STATE>{

.+\n([\t ].*\n)* {
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	return kSubject;
	}

\n {
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	}

. {
	itsCurrentPosition += YYLeng();
	}

}

<FROM_STATE>{

.+\n([\t ].*\n)*	{
	itsCurrentPosition += YYLeng();
	BEGIN(HEADER_STATE);
	return kFrom;
	}

\n {
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	}

. itsCurrentPosition += YYLeng();

}

<DATE_STATE>{

[^[:space:]\n].*	{
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	return kDate;
	}

\n {
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	}

. itsCurrentPosition += YYLeng();

}

<CC_STATE>{

.+\n([\t ].*\n)*	{
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	return kCC;
	}

\n {
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	}

. itsCurrentPosition += YYLeng();

}

<TO_STATE>{

.+\n([\t ].*\n)*	{
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	return kTo;
	}

\n {
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	}

. itsCurrentPosition += YYLeng();

}

<REPTO_STATE>{

.+\n([\t ].*\n)*	{
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	return kReplyTo;
	}

\n {
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	}

. itsCurrentPosition += YYLeng();

}

<STATUS_STATE>{

[^[:space:]\n].*	{
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	return kStatus;
	}

\n {
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	}

. itsCurrentPosition += YYLeng();

}

<MIME_STATE>{

.+\n([\t ].*\n)*	{
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	return kMIME;
	}

\n {
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	}

. itsCurrentPosition += YYLeng();

}

<CONTENT_TYPE_STATE>{

.+\n([\t ].*\n)*	{
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	return kContentType;
	}

\n {
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	}

. itsCurrentPosition += YYLeng();

}

<CONTENT_DISPOSITION_STATE>{

.+\n([\t ].*\n)*	{
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	return kContentDisposition;
	}

\n {
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	}

. itsCurrentPosition += YYLeng();

}

<CONTENT_ENCODING_STATE>{

.+\n([\t ].*\n)*	{
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	return kContentEncoding;
	}

\n {
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	}

. itsCurrentPosition += YYLeng();

}

<REPLY_STATE>{

.+\n([\t ].*\n)*	{
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	return kReplyStatus;
	}

\n {
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	}

. itsCurrentPosition += YYLeng();

}

<X_REPLY_STATE>{

.+\n([\t ].*\n)*	{
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	return kXReplyStatus;
	}

\n {
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	}

. itsCurrentPosition += YYLeng();

}

<PRIORITY_STATE>{

.+\n([\t ].*\n)*	{
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	return kPriority;
	}

\n {
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	}

. itsCurrentPosition += YYLeng();

}

<ACCOUNT_STATE>{

.+\n([\t ].*\n)*	{
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	return kAccount;
	}

\n {
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	}

. itsCurrentPosition += YYLeng();

}

<RECEIVED_STATE>{

.+\n([\t ].*\n)*	{
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	return kReceived;
	}

\n {
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	}

. itsCurrentPosition += YYLeng();

}

<TOSS_STATE>{

\n {
	BEGIN(HEADER_STATE);
	itsCurrentPosition += YYLeng();
	}

. itsCurrentPosition += YYLeng();

}

%%
