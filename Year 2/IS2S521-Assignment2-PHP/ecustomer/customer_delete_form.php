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
		Delete Customer and Property
	</div>
	<div class="row">
		<!-- This form only asks the user the customer id which he desires to delete: -->
		<form class="col s12" method="POST" action="customer_delete_record.php?delete=customer">
			<div class="row">
				<div class="input-field col s4">
					<label>Select Customer ID to delete:</label>
				</div>
				<div class="input-field col s4">
					<input id="cust_id" name="cust_id" type="text" class="validate">
					<label for="cust_id">Customer ID</label>
				</div>
				<div class="input-field col s1">
					<input type="submit" class="btn" value="Delete Customer">
				</div>
			</div>
		</form>
		<!-- This form only asks the user the property id which he desires to delete: -->
		<form class="col s12" method="POST" action="customer_delete_record.php?delete=property">
			<div class="row">
				<div class="input-field col s4">
					<label>Select Property ID to delete:</label>
				</div>
				<div class="input-field col s4">
					<input id="prop_id" name="prop_id" type="text" class="validate">
					<label for="prop_id">Property ID</label>
				</div>
				<div class="input-field col s1">
					<input type="submit" class="btn" value="Delete Property">
				</div>
			</div>
		</form>
	</div>
	
	<!-- Display the database contents so the user can actually pick: -->
	<?php include("display.php"); ?>
	
	<hr>
	<br>
	
	<?php 
		$display_property = True;
		include("display.php"); 
	?>
</div> <!-- Body of page: END -->

<?php include("footer.php"); /* Include footer of website */ ?>
