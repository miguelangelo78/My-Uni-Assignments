<?php
	/* Include essentials: */
	include("is_logged.php"); /* Checks if it's logged in, and redirects if it's not */
	include("dbinfo_inc.php"); /* Grabs database data */
	include("header.php"); /* Loads page header */
?>

<!-- Top navbar -->
<div class="card-action left-align">
	<a href="customer_delete_form.php" class="waves-effect waves-yellow index_btn"><i class="material-icons">replay</i>  Back</a>
	<a href="index.php" class="waves-effect waves-yellow index_btn"><i class="material-icons">airplay</i>  Home</a>
</div> <!-- Top navbar: END -->

<!-- Body of page -->
<div class="card-image">
	<hr>	
	<div class="row chip">
		Delete Customer
	</div>
	<div class="row">
		<div class="col s6 offset-s3">
		<div class="card teal darken-1 z-depth-2">
			<div class="card-content white-text">
				<?php
					if($_GET["delete"] == "customer") {
						/* Delete Customer: */
					
						/* Gather POST data: */
						$customer_id = mysql_real_escape_string($_POST["cust_id"]);
						if(!is_numeric($customer_id)) /* Is customer_id a numeric value */
							die("<br>ERROR: Invalid data inputted: Customer ID must be an integer<br><a href='customer_delete_form.php' class='btn'>Go back</a><br><br>");
					
						/* Grab Property ID first before deleting: */
						$sql = "SELECT propertyID FROM occupancy WHERE customerID='$customer_id'";
						$result = mysql_query($sql);
						$rows = mysql_fetch_row($result);
						$property_id = $rows[0];
						
						/* Now delete Occupancy: */
						$sql = "DELETE FROM Occupancy WHERE customerID='$customer_id'";
						mysql_query($sql) or die("Delete (Occupancy) Error: ".mysql_error());
						
						/* Delete Customer: */
						$sql = "DELETE FROM Customer WHERE customerID='$customer_id'";
						mysql_query($sql) or die("Delete (Customer) Error: ".mysql_error());
						
						/* Delete Property (might not actually delete since multiple customers might use the same property): */
						$sql = "DELETE FROM Property WHERE propertyID='$property_id'";
						mysql_query($sql);
						
						/* Fix the auto increment problem which occurs when we delete something from the database: */
						mysql_query("ALTER TABLE `Occupancy` AUTO_INCREMENT=1");
						mysql_query("ALTER TABLE `Property` AUTO_INCREMENT=1");
						mysql_query("ALTER TABLE `Customer` AUTO_INCREMENT=1");
						
						?> 
							<!-- Show success message: -->
							<h3>Success</h3>
							<hr>
							<p>Customer successfully deleted!</p>
						<?php
					} else
					if($_GET["delete"] == "property") {
						/* Delete Property: */
						$property_id = mysql_real_escape_string($_POST["prop_id"]);
						
						/* Check for clean input: */
						if(!is_numeric($property_id)) /* Is property_id a numeric value */
							die("<br>ERROR: Invalid data inputted: Property ID must be an integer<br><a href='customer_delete_form.php' class='btn'>Go back</a><br><br>");
						
						/* Delete property: */
						mysql_query( "DELETE FROM Property WHERE propertyID='$property_id'") or die("Sorry. Can't delete that property since a client is currently assigned to the same property.<br><div class=\"card-action\"><a href=\"index.php\">Return to Home</a></div>");
						
						?> 
							<!-- Show success message: -->
							<h3>Success</h3>
							<hr>
							<p>Property successfully deleted!</p>
						<?php
					}
					
					/* Finished deleting customer/property: */
					mysql_close();
				?>
			</div>
			<div class="card-action">
				<a href="customer_delete_form.php">Back</a>
				<a href="index.php">Return to Home</a>
			</div>
		</div>
		</div>
	</div>
	<hr>
</div> <!-- Body of page: END -->

<?php include("footer.php"); /* Include footer of website */ ?>
