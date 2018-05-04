<?php
	/* Include essentials: */
	include("is_logged.php"); /* Checks if it's logged in, and redirects if it's not */
	include("dbinfo_inc.php"); /* Grabs database data */
	include("header.php"); /* Loads page header */
?>

<!-- Top navbar -->
<div class="card-action left-align">
	<a href="customer_update_form.php" class="waves-effect waves-yellow index_btn"><i class="material-icons">replay</i>  Back</a>
	<a href="index.php" class="waves-effect waves-yellow index_btn"><i class="material-icons">airplay</i>  Home</a>
</div> <!-- Top navbar: END -->

<!-- Body of page -->
<div class="card-image">
	<hr>	
	<div class="row chip">
		Update Customer
	</div>
	<div class="row">
		<div class="col s6 offset-s3">
		<div class="card teal darken-1 z-depth-2">
			<div class="card-content white-text">
				<?php
					/* Gather POST data: */
					$customer_id = $_SESSION['cust_id']; /* No need to check if it's numeric, since we did that on the previous page */
					/* Protect this page if the customer_id is not set: */
					if(!isset($customer_id) || empty($customer_id))
						header("location:index.php"); 
					
					/* Grab form information from POST: */
					$first_name = mysql_real_escape_string($_POST["first_name"]);
					$last_name = mysql_real_escape_string($_POST["last_name"]);
					$street = mysql_real_escape_string($_POST["street"]);
					$street_no = mysql_real_escape_string($_POST["street_no"]);
					$start_date = mysql_real_escape_string($_POST["start_date"]);
					$end_date = mysql_real_escape_string($_POST["end_date"]);
					
					/* Check for clean inputs: */
					if(!is_numeric($street_no)) /* Is street number numeric */
						die("Invalid data inputted: street number must be integer<br><a href='customer_update_form.php' class='btn'>Go back</a>");

					if(DateTime::createFromFormat('d-M-y', $start_date) == FALSE) /* Is start date an actual date */
						die("Invalid data inputted: start date must be a valid date (format: dd-Mmm-yy)<br><a href='customer_update_form.php' class='btn'>Go back</a>");
					
					if(!empty($end_date) && DateTime::createFromFormat('d-M-y', $end_date) == FALSE) /* Is end_date an actual date */
						die("Invalid data inputted: end date must be a valid date (format: dd-Mmm-yy)<br><a href='customer_insert_form.php' class='btn'>Go back</a>");
					if(empty($end_date))
						$end_date = NULL;
						
					/* Update Customers: */
					$sql="UPDATE Customer SET firstName='$first_name', lastName='$last_name' 
						WHERE customerID='".$customer_id."'";
					mysql_query($sql) or die("Update (Customers) Error: ".mysql_error());
					
					/* Update Property: */
					$sql = "UPDATE Property SET propertyStreet='$street', propertyNo='$street_no' 
						WHERE propertyNo='".$_SESSION['old_street_id']."'";
					mysql_query($sql) or die("Update (Property) Error: ".mysql_error());
					
					/* Update Occupancy: */
					$sql="UPDATE Occupancy SET occupancyStart=STR_TO_DATE('$start_date', '%d-%b-%y'), occupancyEnd=STR_TO_DATE('$end_date','%d-%b-%y')
						WHERE customerID='".$customer_id."'";
					mysql_query($sql) or die("Update (Occupancy) Error: ".mysql_error());
					
					/* Finished updating: */
					mysql_close();
				?>
				<!-- Show success message: -->
				<h3>Success</h3>
				<hr>
				<p>Customer successfully updated!</p>
			</div>
			<div class="card-action">
				<a href="customer_update_form.php">Back</a>
				<a href="index.php">Return to Home</a>
			</div>
		</div>
		</div>
	</div>
	<hr>
</div> <!-- Body of page: END -->

<?php include("footer.php"); /* Include footer of website */ ?>
