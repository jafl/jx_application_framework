059
$Λ

=begin
doc
=end

$a = /abcd/i
%r(abcd)i

'a\'"#{c}b'
"a\"'#{c}b"
$f=<<ABC;
foobar #{c}
ABC

`ab$b#{c}`
%x{ab#{c}c} foo
%w{a b c}
