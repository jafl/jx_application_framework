#!/usr/bin/perl

use strict;

my @commands;

chomp(my $s = `find . -name '*.o.json' -a ! -path '*/test/*'`);
for my $f (split(/\n/, $s))
	{
	open(F, '< '.$f);
	local $/;
	(my $cmd = <F>) =~ s/,\n?$//;
	$cmd =~ s/"(-clang-vendor-feature=.+?|-fno-odr-hash-protocols)",?//g;
	push(@commands, $cmd);
	close(F);
	}

open(F, '> compile_commands.json');
print F '[',join(',', @commands),']';
close(F);

system "run-clang-tidy.py -checks='-*,hicpp-use-auto' -quiet -fix";

system "run-clang-tidy.py -checks='-*,performance-*,portability-*' -quiet";
