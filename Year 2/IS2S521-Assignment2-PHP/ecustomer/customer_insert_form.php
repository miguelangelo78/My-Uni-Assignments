<?php 
	/* Include essentials: */
	include("is_logged.php"); /* Checks if it's logged in, and redirects if it's not */
	include("dbinfo_inc.php"); /* Grabs database data */
	include("header.php"); /* Loads page header */
?>
	    
<!-- Top navbar -->
<div class="card-action left-align">
	<a href="index.php" class="waves-effect waves-yellow index_btn"><i class="material-icons">airplay</i>  Home</a>
</div> <!-- Top navbar: END -->

<!-- Body of page -->
<div class="card-image">
	<hr>	
	<div class="row chip">
		Insert New Customer
	</div>
	
	<!-- Similarly to 'display.php', just include 'data_form.php'. 
		It's got everything ready in it to request the user for customer data: -->
	<?php include("data_form.php"); ?>

	<hr>
	<br>
	<div class="row chip">
		Insert New Property
	</div>
	
	<?php 
		$insert_property = True;
		include("data_form.php"); 
	?>

	<hr>
</div> <!-- Body of page: END -->

<?php include("footer.php"); /* Include footer of website */ ?>
