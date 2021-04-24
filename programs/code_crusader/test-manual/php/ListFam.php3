<html>
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
</head>
<body text="#000000" bgcolor="#999999" link="#333333" vlink="#333333" alink="#CCCCCC" background="images/bgklfs2.gif" nosave>
<?php
    printf("id=%s enreg=%s", $id, $enreg);

	$db = mysql_connect("localhost", "root");
	mysql_select_db("LFSeoul",$db);

	if ($id)
	{
?>
<center><h1>EDITION D'UNE FICHE DE FAMILLE<p></h1></center>

<>
<?php %>
<>

<?php
		if ($enreg)
		{
?>
<center>MODIFICATION EFFECTUEE<p></h1></center>
<?php
			$sql = "UPDATE Famille SET NomPere='$nomp', PrenomP='$prenomp' WHERE ID='$id'";
			$result = mysql_query($sql);

			$sql = "SELECT * FROM Famille WHERE ID='$id'";
			$result = mysql_query($sql);
			$myrow = mysql_fetch_array($result);
			$id = $myrow["ID"];
			$nomp = $myrow["NomPere"];
			$prenomp = $myrow["PrenomP"];
			printf("<a href=\"%s?id=%s\">- - %s ( %s )</a><br>", $PHP_SELF, $id, $nomp, $prenomp);
			printf("<center><a href=\"%s\">Retourner a la liste</a></center>", $PHP_SELF);
		}
		else
		{
			$sql = "SELECT * FROM Famille WHERE ID='$id'";
			$result = mysql_query($sql);
			$myrow = mysql_fetch_array($result);
			$id = $myrow["ID"];
			$nomp = $myrow["NomPere"];
			$prenomp = $myrow["PrenomP"];
?>
<form method="post" action="<?php echo $PHP_SELF ?>">
<input type=hidden name="id" value="<?php echo $id ?>">
<center>Nom du Pere	 :<input type=text name="nomp" value="<?php echo $nomp ?>">
Prenom :<input type=text name="prenomp" value="<?php echo $prenomp ?>"><br>
<input type=submit name="enreg" value="Enregistrer"></center>
</form><p>
<?php
			printf("<center><a href=\"%s\">Retourner a la liste</a></center>", $PHP_SELF);
		}
	}
	else
	{
?>
<center><h1>LISTE DES FAMILLES<p></h1></center>
<?php
		$sql = "SELECT * FROM Famille";
		$result = mysql_query($sql);
		while ($myrow = mysql_fetch_array($result))
		{ 
			$id = $myrow["ID"];
			$nomp = $myrow["NomPere"];
			$prenomp = $myrow["PrenomP"];
			printf("<a href=\"%s?id=%s\">- - %s ( %s )</a><br>", $PHP_SELF, $id, $nomp, $prenomp);
		}
	}
	printf("<p><center><a href=\"%s\">Nouvelle famille</a></center>", $PHP_SELF);
?>
</body>
</html>
