<?php
require_once('../../lib/debug.inc');

/* test */

ob_start();
phpinfo();
$s = ob_get_clean();

// test

$x = `cat ${y}`;

# test

$z = <<<EOF
foo $bar ${baz} ${}
EOF;

$n = <<<'EOF'
foo $bar ${baz}
EOF;
?>

<html>
<?php echo "{$} $s ${abc[3]} {$foo[1][3]->bar[2]->baz['zot']} {${$object->getName()}}"; ?>

<script>
(function(){
"use strict";

	/* test */

	window.addEventListener('load', () =>
	{
		return;
	});

	// test

})();
</script>
</html>
