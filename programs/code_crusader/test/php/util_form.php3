<?php

function PrintMenuItem($line, $selected)
{
	if (ereg("(..) (.+)", $line, $match))
		{
		echo "<option value=$match[1]";
		if ($selected)
			{
			echo " selected";
			}
		echo ">$match[2]";
		}
}

function BuildMenu($file)
{
	$data = `cat ../../data/form/$file`;
	$line = split("\n", $data);

	PrintMenuItem($line[0], 1);
	for ($i=1; $i<sizeof($line); $i++)
		{
		PrintMenuItem($line[$i], 0);
		}
}

function PrintPurchaseLinks($product)
{
	if ($product->Type == 'Demo'): ?>

	<a href="https://stream.topchoice.com/cgi-bin/newplanetsoftware/demo/<?php echo $product->DownloadFile ?>?product_id=<?php echo $product->ID ?>">Download</a>

	<?php elseif ($product->Type == 'Download'): ?>

	<a href="https://stream.topchoice.com/cgi-bin/newplanetsoftware/sell/<?php echo $product->DownloadFile ?>?product_id=<?php echo $product->ID ?>&pay_method=credit">Credit&nbsp;card</a> |
	<a href="https://stream.topchoice.com/cgi-bin/newplanetsoftware/sell/<?php echo $product->DownloadFile ?>?product_id=<?php echo $product->ID ?>&pay_method=check">On-line&nbsp;check</a>

	<?php elseif ($product->Type == 'Ship'): ?>

	<a href="https://stream.topchoice.com/cgi-bin/newplanetsoftware/ship/<?php echo $product->DownloadFile ?>?product_id=<?php echo $product->ID ?>&pay_method=credit">Credit card</a> |
	<a href="https://stream.topchoice.com/cgi-bin/newplanetsoftware/ship/<?php echo $product->DownloadFile ?>?product_id=<?php echo $product->ID ?>&pay_method=check">On-line check</a>

	<?php endif;
}

function PrintDiscountLinks($product)
{
	if ($product->Type == 'Download'): ?>

	<a href="https://stream.topchoice.com/cgi-bin/newplanetsoftware/sell_discount/<?php echo $product->DownloadFile ?>?product_id=<?php echo $product->ID ?>&pay_method=credit">Credit&nbsp;card</a> |
	<a href="https://stream.topchoice.com/cgi-bin/newplanetsoftware/sell_discount/<?php echo $product->DownloadFile ?>?product_id=<?php echo $product->ID ?>&pay_method=check">On-line&nbsp;check</a>

	<?php elseif ($product->Type == 'Ship'): ?>

	<a href="https://stream.topchoice.com/cgi-bin/newplanetsoftware/ship_discount/<?php echo $product->DownloadFile ?>?product_id=<?php echo $product->ID ?>&pay_method=credit">Credit card</a> |
	<a href="https://stream.topchoice.com/cgi-bin/newplanetsoftware/ship_discount/<?php echo $product->DownloadFile ?>?product_id=<?php echo $product->ID ?>&pay_method=check">On-line check</a>

	<?php endif;
}

?>
