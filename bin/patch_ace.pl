#!/usr/bin/perl

use strict;

my $path;
if (!($path = $ARGV[0]))
	{
	print "usage: $0 path_to_source\n";
	exit;
	}

#
# config-g++-common.h
#

my $file = $path.'/config-g++-common.h';

my $text;
open(F, '< '.$file);
{
local $/;
$text = <F>;
}
close(F);

if ($text =~ s|\n\n#define ACE_NEW_THROWS_EXCEPTIONS\n(.+?)\n\n|\n\n#if defined (__EXCEPTIONS)\n#define ACE_NEW_THROWS_EXCEPTIONS\n$1\n#endif /\* __EXCEPTIONS \*/\n\n|s)
	{
	open(F, '> '.$file);
	print F $text;
	close(F);
	}
