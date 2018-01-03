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
	<div class="row">
		<div class="col s6 offset-s3">
			<div class="card teal darken-1 z-depth-2">
				<div class="card-content white-text">
					<?php
						/* Gather POST data: */
						$first_name = mysql_real_escape_string($_POST["first_name"]);
						$last_name = mysql_real_escape_string($_POST["last_name"]);
						$street = trim(mysql_real_escape_string($_POST["street"]));
						$street_no = mysql_real_escape_string($_POST["street_no"]);
						$start_date = mysql_real_escape_string($_POST["start_date"]);
						$end_date = mysql_real_escape_string($_POST["end_date"]);
						
						/* Check for clean inputs: */
						if(!is_numeric($street_no)) /* Is street_no a number */
							die("Invalid data inputted: street number must be integer<br><a href='customer_insert_form.php' class='btn'>Go back</a>");

						if(DateTime::createFromFormat('d-M-y', $start_date) == FALSE) /* Is start_date an actual date */
							die("Invalid data inputted: start date must be a valid date (format: dd-Mmm-yy)<br><a href='customer_insert_form.php' class='btn'>Go back</a>");
							
						if(!empty($end_date) && DateTime::createFromFormat('d-M-y', $end_date) == FALSE) /* Is end_date an actual date */
							die("Invalid data inputted: end date must be a valid date (format: dd-Mmm-yy)<br><a href='customer_insert_form.php' class='btn'>Go back</a>");
						if(empty($end_date))
							$end_date = NULL;
						
						/* Fetch last row from tables Customer and Property: */
						$result = mysql_query("SELECT MAX(customerID) FROM customer");
						$rows = mysql_fetch_row($result);
						$last_customer_id = $rows[0] + 1; /* last customer */
						
						/* Insert into Customers: */
						$sql = "INSERT INTO customer (firstName, lastName) VALUES ('$first_name','$last_name')";
						mysql_query($sql) or die("Insert (Customer) Error: ".mysql_error());		
						
						/* Insert into Property: */
						
						/* First we'll check if this property already exists: */
						$result = mysql_query("SELECT count(*) as count FROM property 
									WHERE propertyStreet = '$street' AND propertyNo =	$street_no");
						$count = mysql_fetch_assoc($result);
						
						$last_prop_id = 0; /* Forward declare this variable */
						if($count['count'] == 0) {
							/* If the property doesn't exist, then we insert a new one */
							$result = mysql_query("SELECT MAX(propertyID) FROM property");
							$rows = mysql_fetch_row($result);
							$last_prop_id = $rows[0] + 1; /* next available last property's ID */
						
							$result = mysql_query("SELECT MAX(propertyID) FROM property");
							$rows = mysql_fetch_row($result);
							$sql="INSERT INTO Property VALUES ($last_prop_id,'$street_no','$street')";
							mysql_query($sql) or die("Insert (Property) Error: ".mysql_error());
						} else {
							/* Else the property exists, we need to fetch its ID in order to be able to insert into Occupancy */
							$result = mysql_query("SELECT propertyID FROM property WHERE propertyStreet = '$street'");
							$row = mysql_fetch_assoc($result);
							$last_prop_id = $row["propertyID"];
						}

						/* Insert into Occupancy: */
						$sql="INSERT INTO Occupancy (propertyID, customerID, occupancyStart, occupancyEnd) 
							  VALUES ('$last_prop_id', $last_customer_id, STR_TO_DATE('$start_date', '%d-%b-%y'), STR_TO_DATE('$end_date', '%d-%b-%y'))";
						mysql_query($sql) or die("Insert (Occupancy) Error: ".mysql_error());
						
						/* New customer successfully inserted */
						mysql_close();
					?>
					
					<!-- Show success message: -->
					<h3>Success</h3>
					<hr>
					<p>New Customer successfully added!</p>
				</div>
				<div class="card-action">
					<a href="index.php">Return to Home</a>
				</div>
			</div>
		</div>
	</div>
	<hr>
</div> <!-- Body of page: END -->

<?php include("footer.php"); /* Include footer of website */ ?>