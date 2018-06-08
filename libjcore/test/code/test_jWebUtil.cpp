/******************************************************************************
 test_jWebUtil.cc

	Program to test jWebUtil.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include <jWebUtil.h>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(URL)
{
	JAssertTrue(JIsURL(JString("http://google.com", kJFalse)));
	JAssertTrue(JIsURL(JString("https://google.com", kJFalse)));
	JAssertTrue(JIsURL(JString("http://google.com/test", kJFalse)));
	JAssertFalse(JIsURL(JString("/usr/local", kJFalse)));

	JString protocol, host, path;
	JIndex port;
	JAssertTrue(JParseURL(
		JString("http://google.com", kJFalse),
		&protocol, &host, &port, &path));
	JAssertStringsEqual("http", protocol);
	JAssertStringsEqual("google.com", host);
	JAssertEqual(0, port);
	JAssertStringsEqual("/", path);

	JAssertTrue(JParseURL(
		JString("https://google.com:8443/foo/bar", kJFalse),
		&protocol, &host, &port, &path));
	JAssertStringsEqual("https", protocol);
	JAssertStringsEqual("google.com", host);
	JAssertEqual(8443, port);
	JAssertStringsEqual("/foo/bar", path);

	JAssertFalse(JParseURL(
		JString("/usr/local", kJFalse),
		&protocol, &host, &port, &path));
}
