#!/bin/csh

echo ${}
echo $14
echo ar$x
echo ar${x}
cat <<ar\$x

x=`cat ~/${foo}`
echo $x

${A[$i]}
echo "x${A[$i]}i"

 $* $ $$ $@ $% $3 \$4 ${039823#abc} ${#xyz} ${} ${xy+z}
"$* $ $$ $@ $% $3 \$4 ${039823#abc} ${#xyz} ${} ${xy+z}"
'$* $ $$ $@ $% $3 \$4 ${039823#abc} ${#xyz} ${} ${xy+z}'
