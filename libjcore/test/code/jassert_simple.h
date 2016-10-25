// for unit tests that do not load the entire libjcore

int
JAssert
	(
	const char*	expr,
	const char*	file,
	const int	line
	)
{
	JUnitTestManager::ReportFatal(expr, file, line);
	return 0;
}

void
JLocateDelete
	(
	const JUtf8Byte* file,
	const JUInt32    line
	)
{
	// no-op
}

void*
operator new
	(
	size_t           size,
	const JUtf8Byte* file,
	const JUInt32    line
	)
	noexcept
{
	return malloc(size+1);
}

void*
operator new[]
	(
	size_t           size,
	const JUtf8Byte* file,
	const JUInt32    line
	)
	noexcept
{
	return malloc(size+1);
}
