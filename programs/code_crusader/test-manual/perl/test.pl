#!/usr/bin/perl

use strict;

require('../shared/general.pl');

sub abc($\@$*\%\$;%)
{
	my ($abc, %xyz) = @_;
	print 2.0 / 0x3 + 534 + 0 - 0b101 + 0b2 - 0xG + 063 - 081 + &$abc;
	$vers = v0.2 + v33 + v1.2.3.4.5;
	$v2   = 0.3.5;
}

if (/$a${bc}k/)
	{
	s/5$a3_8/$x/giw;
	}

# single line comment

=comment

This is POD!

=head1

Heading

=cut

print
if
continue
socketpair
die
goto

00
$test
@test
%test
\$test
abc
x = 3 ? 5 : 7;

${abc->{$x[5]}}
${a}
@{abc}
%{xyz}
&{foo}
%$${xyz}
${ }
${}
\%abc
\%$xyz
\@x
\&foo

$data = <<EOD;
this is a string
EOD

$data = <<"EOD";
this is a string
EOD

$data = <<'EOD';
this is a string
EOD

$data = <<`EOD`;
this is a string
EOD

$[ $] $\ $; $@ $! $"" $* $` $' $? $. $$ $@ $^E $^Z $} $-

$3
$$xyz
@$abc # comment
%$abc

#line 1 "abc"
# line 18
#line

q|a$bc|;
'$a';
qq[a${b}$c[3]*&];
"a${b}$c+";
qr+123+mz;
qx(echo $b ${x->{z}} hel[(\))lo);
`ls $dir`;
qw\abc\;
$x ? 3 : 5.0;
if (?abc?)
m%xy%isl;
5/3;
die if /s/xz;
5 s/$a/$b/gw;
s<a<>b$c><x\<\>$$yz$${xy}e2>;
s(\(\))((3))gt;
tr/a/b/cj;
y\ab\c\;

split(/=/);

__DATA__
This is module data that can be read via main::DATA
123
456
