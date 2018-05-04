<?php 
	/* Include essentials: */
	include("is_logged.php"); /* Checks if it's logged in, and redirects if it's not */
	include("dbinfo_inc.php"); /* Grabs database data */
	include("header.php"); /* Loads page header */
?>

<!-- Top navbar -->
<div class="card-action left-align" style='margin-bottom:-25px'>
	<center>
	<a style='font-size:12px' href="index.php" class="waves-effect waves-yellow index_btn btn">Home</a>
	<br>
	<br>
	<span class='row chip' style='font-size:13px'>Display:</span>
	<?php if($_GET["display"] !== "history") { ?>
	<a style="font-size:13px" href="display_page.php?display=history" class="waves-effect waves-yellow index_btn">History Page</a>
	<?php } if($_GET["display"] !== "customers") { ?>
	<a style="font-size:13px" href="display_page.php?display=customers" class="waves-effect waves-yellow index_btn">Customers</a>
	<?php } if($_GET["display"] !== "occupations") { ?>
	<a style="font-size:13px" href="display_page.php?display=occupations" class="waves-effect waves-yellow index_btn">Occupations</a>
	<?php } if($_GET["display"] !== "properties") { ?>
	<a style="font-size:13px" href="display_page.php?display=properties" class="waves-effect waves-yellow index_btn">Properties</a>
	<?php } ?>
	</center>
</div> <!-- Top navbar: END -->

<!-- Body of page -->
<div class="card-image">
	<hr>	

	<!-- Just include 'display.php', since it's got everything in it ready to display the database:  -->
	<?php include("display.php"); ?>
	
	<hr>
	<br>
</div> <!-- Body of page: END -->

<?php include("footer.php"); /* Include footer of website */ ?>