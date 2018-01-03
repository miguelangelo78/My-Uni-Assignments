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
		Update Customer
	</div>

	<div class="row">
		<!-- This form only asks the user the customer id which he desires to update: -->
		<form class="col s12" method="POST" action="customer_change_form.php">
		<div class="row">
			<div class="input-field col s4">
				<label>Select Customer ID to update:</label>
			</div>
			<div class="input-field col s5">
				<input id="cust_id" name="cust_id" type="text" class="validate">
			  	<label for="cust_id">Customer ID</label>
			</div>
			<div class="input-field col s1">
				<input type="submit" class="btn" value="Update">
			</div>
		</div>
		</form>
	</div>
	
	<!-- Display the database contents so the user can actually pick: -->
	<?php include("display.php"); ?>
	
	</div>
	<hr>
	<br>
</div> <!-- Body of page: END -->

<?php include("footer.php"); /* Include footer of website */ ?>
