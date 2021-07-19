#!/usr/bin/perl

use strict;

my @commands;

my @s = glob('code/*.o.json');
for my $f (@s)
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

`run-clang-tidy.py -checks='-*,hicpp-use-auto' -quiet -fix`;

system "run-clang-tidy.py -checks='-*,performance-*,portability-*' -quiet";
