/******************************************************************************
 test_jWebUtil.cc

	Program to test jWebUtil.

	Written by John Lindal.

 *****************************************************************************/

#include <JUnitTestManager.h>
#include <jWebUtil.h>
#include <jAssert.h>

int main()
{
	return JUnitTestManager::Execute();
}

JTEST(URL)
{
	JAssertTrue(JIsURL(JString("http://google.com", 0, kJFalse)));
	JAssertTrue(JIsURL(JString("https://google.com", 0, kJFalse)));
	JAssertTrue(JIsURL(JString("http://google.com/test", 0, kJFalse)));
	JAssertFalse(JIsURL(JString("/usr/local", 0, kJFalse)));

	JString protocol, host, path;
	JIndex port;
	JAssertTrue(JParseURL(
		JString("http://google.com", 0, kJFalse),
		&protocol, &host, &port, &path));
	JAssertStringsEqual("http", protocol);
	JAssertStringsEqual("google.com", host);
	JAssertEqual(0, port);
	JAssertStringsEqual("/", path);

	JAssertTrue(JParseURL(
		JString("https://google.com:8443/foo/bar", 0, kJFalse),
		&protocol, &host, &port, &path));
	JAssertStringsEqual("https", protocol);
	JAssertStringsEqual("google.com", host);
	JAssertEqual(8443, port);
	JAssertStringsEqual("/foo/bar", path);

	JAssertFalse(JParseURL(
		JString("/usr/local", 0, kJFalse),
		&protocol, &host, &port, &path));
}
