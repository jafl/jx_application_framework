#!/usr/bin/perl

use strict;

my $file;
if (!($file = $ARGV[0]))
	{
	print "usage: $0 file_name\n";
	exit;
	}

my $old_text;
open(F, '< '.$file);
{
local $/;
$old_text = <F>;
}
close(F);

exit if ($old_text !~ m|/\* Prevent warnings from -Wmissing-prototypes\.  \*/|s);

$old_text    =~ s|^(.+)/\* Prevent warnings from -Wmissing-prototypes\.  \*/||s;
my $new_text =  $1;

$old_text  =~ m|/\* ! YYPARSE_PARAM \*/([^*])+/\* ! YYPARSE_PARAM \*/(.+)$|s;
$new_text .=  $2;

open(F, '> '.$file);
print F $new_text;
close(F);
