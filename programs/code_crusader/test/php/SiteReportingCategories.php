<?
require_once('portal_header.php');
require_once('site_reporting_category.php');

$table_form_name       = 'edit_reporting_category_form';
$table_subsection_name = 'reporting_category_table';
$edit_action_name      = 'category_edit';
$delete_action_name    = 'category_delete';

// init with value from popup
$status_msg = $_GET[$table_form_name];
unset($_GET[$table_form_name]);

$new_category_fields = array
(
	'category_name' => array
	(
		'type'      => 'text',
		'label'     => 'Reporting Category',
		'req'       => true,
		'maxlength' => 200,
		'size'      => 60,
		'button'    => '<input type="submit" name="create_category" value="Create" class="ypnFormButton">'
	)
);

$pagination = new YPNPagination($table_form_name, 'siteReportingCategory', array
(
	new YPNColumnTitle('<input name="check_all" type="checkbox" onClick="YAHOO.YPN.SiteReportingCategory.toggleAll(this);">', YPN_CB_COLUMN),
	new YPNColumnTitle('Reporting Category', YPN_TEXT_COLUMN, '_CompareReportingCategories', true),
	new YPNColumnTitle('Active', YPN_IMAGE_COLUMN)
));

if ($pagination->ProcessForm())
{
	// work has been done
}

// create category

elseif ($_POST['create_category'] && validateFormFields($new_category_fields))
{
	$result = CreateSiteReportingCategory($ypn_cookie->GetAccountId(), $_POST['category_name']);
	$new_category_fields['category_name']['err'] = GetSiteReportingCategoryErrorMsg($result, 'creating');

	if ($new_category_fields['category_name']['err'] == '')
	{
		clearFormFields($new_category_fields);
	}
}

// edit category

elseif ($_POST['action'] == $edit_action_name)
{
	if (!is_array($_POST['active_category_id']) && !is_array($_POST['inactive_category_id']))
	{
		$status_msg = 'Please select a Reporting Category to rename.';
	}
	elseif (count($_POST['active_category_id']) + count($_POST['inactive_category_id']) > 1)
	{
		$status_msg = 'Please select only one Reporting Category to rename.';
	}
}

// delete categories

elseif ($_POST['action'] == $delete_action_name)
{
	if (!is_array($_POST['active_category_id']) && !is_array($_POST['inactive_category_id']))
	{
		$status_msg = 'Please select at least one Reporting Category to delete.';
	}
	else
	{
		$ids = array_merge(($_POST['active_category_id'] ? $_POST['active_category_id'] : array()),
					       ($_POST['inactive_category_id'] ? $_POST['inactive_category_id'] : array()));
		$result = DeleteSiteReportingCategories($ypn_cookie->GetAccountId(), $ids);
		if ($result['Status'] != 'OK')
		{
			$status_msg = 'There was an error deleting a Reporting Category.';
		}
	}
}

// activate/deactivate categories

elseif (is_array($_POST['active_category_id']) && isset($_POST['inactive_category_id']))
{
	$result = SetSiteReportingCategoriesActive($ypn_cookie->GetAccountId(), $_POST['active_category_id'] + $_POST['inactive_category_id'], isset($_POST['activate']));
	$status_msg = GetSiteReportingCategoryErrorMsg($result, (isset($_POST['activate']) ? 'activating' : 'deactivating'));
}
elseif (is_array($_POST['active_category_id']) && isset($_POST['deactivate']))
{
	$result = SetSiteReportingCategoriesActive($ypn_cookie->GetAccountId(), $_POST['active_category_id'], false);
	$status_msg = GetSiteReportingCategoryErrorMsg($result, 'deactivating');
}
elseif (is_array($_POST['inactive_category_id']) && isset($_POST['activate']))
{
	$result = SetSiteReportingCategoriesActive($ypn_cookie->GetAccountId(), $_POST['inactive_category_id'], true);
	$status_msg = GetSiteReportingCategoryErrorMsg($result, 'activating');
}
else if (is_array($_POST['active_category_id']) || isset($_POST['activate']))
{
	$status_msg = 'Please select at least one inactive Reporting Category to activate.';
}
else if (is_array($_POST['inactive_category_id']) || isset($_POST['deactivate']))
{
	$status_msg = 'Please select at least one active Reporting Category to deactivate.';
}

$categories = GetSiteReportingCategories($ypn_cookie->GetAccountId(), true);

$show_list  = array('Active', 'Inactive', 'All');
if ($_REQUEST['show'])
{
	if ($_REQUEST['show'] != $ypn_state->Get(YPN_STATE_VIEW_TYPE))
	{
		$ypn_state->Set(YPN_STATE_PAGE_INDEX, 1);
	}
	$ypn_state->Set(YPN_STATE_VIEW_TYPE, $_REQUEST['show']);
}
$_REQUEST['show'] = $ypn_state->Get(YPN_STATE_VIEW_TYPE);

$count = CountActiveInactive($categories);
$pagination->PaginateActiveInactive($_REQUEST['show'], $count['active'], $count['inactive']);
$pagination->SaveState();

function _CompareReportingCategories($a, $b)
{
	return strcasecmp($a['Name'], $b['Name']);
}

PrintPortalHeader();
?>

<script type="text/javascript">
var s = 'xyz';
s.replace(/x(.)z/, 'a$1b') < 3;
YAHOO.YPN.SiteReportingCategory =
{
	categoryList: {},

	editCategory: function ()
	{
		var f = document.<?= $table_form_name ?>;
		var activeIdList   = YAHOO.YPN.Form.getCheckedItems(f['active_category_id[]']);
		var inactiveIdList = YAHOO.YPN.Form.getCheckedItems(f['inactive_category_id[]']);
		if (activeIdList.length == 0 && inactiveIdList.length == 0)
		{
			f.action.value='<?= $edit_action_name ?>';
			f.submit();
			return;
		}

		var idList = activeIdList.concat(inactiveIdList);
		if (idList.length > 1)
		{
			f.action.value='<?= $edit_action_name ?>';
			f.submit();
			return;
		}

		YAHOO.YPN.Layout.openTB('/tool/edit_site_category.php?form=<?= $table_form_name ?>&edit_id='+idList[0],400,150);
	},

	deleteCategory: function ()
	{
		var f              = document.<?= $table_form_name ?>;
		var activeIdList   = YAHOO.YPN.Form.getCheckedItems(f['active_category_id[]']);
		var inactiveIdList = YAHOO.YPN.Form.getCheckedItems(f['inactive_category_id[]']);
		if (activeIdList.length == 0 && inactiveIdList.length == 0)
		{
			f.action.value='<?= $delete_action_name ?>';
			f.submit();
			return;
		}

		YAHOO.YPN.Layout.openTB('/tool/site_reporting_category_delete_confirm.php?form=<?= $table_form_name ?>&action=<?= $delete_action_name ?>',400,100);
	},

	checkAll: function ()
	{
		var f = document.<?= $table_form_name ?>;
		YAHOO.YPN.Form.checkBoxes(f['active_category_id[]'], true);
		YAHOO.YPN.Form.checkBoxes(f['inactive_category_id[]'], true);
		f.check_all.checked = true;
	},

	clearAll: function ()
	{
		var f = document.<?= $table_form_name ?>;
		YAHOO.YPN.Form.checkBoxes(f['active_category_id[]'], false);
		YAHOO.YPN.Form.checkBoxes(f['inactive_category_id[]'], false);
		f.check_all.checked = false;
	},

	toggleAll: function (cb)
	{
		if (cb.checked)
		{
			this.checkAll();
		}
		else
		{
			this.clearAll();
		}
	}
};

YAHOO.util.Event.addListener(window, 'load', function() { YAHOO.YPN.SiteReportingCategory.clearAll() });
<?
// passing Categories to New/Edit/Delete popup

if (is_array($categories))
{
	foreach ($categories as $item)
	{
		echo 'YAHOO.YPN.SiteReportingCategory.categoryList['.$item['RowId'].'] = "'.TrimLabel($item['Name'], 50).'";';
	}
}
?>
</script>

<?= BeginSectionFrame() ?>
<p class="ypnNote">Create categories to define custom reports on the performance of the ads on your site.
<? if (is_array($categories) && count($categories) > 0): ?>
	<br><br>
	<b>Just defined a new Reporting Category? Don't forget to <a href="/portal/cm/SiteAdLayout.php">update your ad code</a> and paste it into your site!</b>
<? endif; ?>
</p>
<?= EndSectionFrame() ?>

<?= requiredFieldsNote() ?>

<?= BeginSectionFrame() ?>

<h3>Create a Reporting Category</h3>

<form action="<?= $_SERVER['PHP_SELF'] ?>#" method="POST">
<?= buildFormFields($new_category_fields); ?>  
</form>

<?= BeginTip(105, 10) ?>
To use your new category, <a href="/portal/cm/SiteAdLayout.php">update your ad code</a> and paste it into your site.</b>
<?= EndTip() ?>

<?= SectionDivider() ?>

<a name="<?= $table_subsection_name ?>"></a>

<h3>Manage Reporting Categories</h3>

<?= StatusMessage($status_msg) ?>

<form action="<?= $_SERVER['PHP_SELF'] ?>#<?= $table_subsection_name ?>" name="<?= $table_form_name ?>" method="POST">
<input type="hidden" name="action" value="">
<?
$table_buttons =
	'<input type="button" value="Rename" onclick="YAHOO.YPN.SiteReportingCategory.editCategory('.$table_form_name.'); return false;" class="ypnTableButton" />'.
	'&nbsp;'.
	'<input type="button" value="Delete" onclick="YAHOO.YPN.SiteReportingCategory.deleteCategory('.$table_form_name.'); return false;" class="ypnTableButton" />'.
	'&nbsp;&nbsp;&nbsp;'.
	'<input type="submit" name="activate" value="Mark as Active" class="ypnTableButton" />'.
	'&nbsp;'.
	'<input type="submit" name="deactivate" value="Mark as Inactive" class="ypnTableButton" />';

echo $pagination->TableHeader
(
	# first row
	'View: '.ViewAsLinks($table_subsection_name, $_REQUEST['show'], $show_list),

	# second row
	$table_buttons
);

if (!is_array($categories) || count($categories) == 0)
{
?>
	<tr><td class="ypnEmptyTable ypnNoRtBdr" colspan=<?= $pagination->GetColumnCount() ?>>
	You have not created any Reporting Categories.
	</td></tr>
<?
}
else
{
	$pagination->Sort($categories);

	$count = 1;
	foreach ($categories as $item)
	{
		if (($_REQUEST['show'] != 'Inactive' && $item['Active'] == 'true') ||
			($_REQUEST['show'] != 'Active' && $item['Active'] == 'false'))
		{
			if ($count >= $pagination->GetFirstRow())
			{
?>
				<tr class="<?= ($count % 2 == 0 ? 'ypnColorEven' : 'ypnColorOdd') ?>">
				<td class="ypnCheckBox"><input type="checkbox" name="<?= ($item['Active'] == 'true' ? 'active' : 'inactive') ?>_category_id[]" value="<?= $item['RowId'] ?>"></td>
				<td><?= TrimLabel($item['Name'], 50) ?></td>
				<td class="<?= ($item['Active'] == 'true' ? 'ypnActive' : 'ypnInactive') ?> ypnNoRtBdr">&nbsp;</td>
				</tr>
<?
			}
			$count++;
			if ($count > $pagination->GetLastRow())
			{
				break;
			}
		}
	}
}

echo $pagination->TableFooter
(
	# first row
	$table_buttons,

	# second row
	'<a href="javascript:YAHOO.YPN.SiteReportingCategory.checkAll();">Check All</a>'.
	'&nbsp;-&nbsp;'.
	'<a href="javascript:YAHOO.YPN.SiteReportingCategory.clearAll();">Clear All</a>'
);
?>

</form>

<?= BeginTip() ?>
The maximum number of active Reporting Categories is 100.
<?= EndTip() ?>

<?= EndSectionFrame() ?>

<? PrintPortalFooter(); ?>