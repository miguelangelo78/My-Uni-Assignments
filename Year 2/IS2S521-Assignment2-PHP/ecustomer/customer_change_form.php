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

<!-- Body of form -->
<div class="card-image">
	<hr>
	<div class="row chip">
		Update Customer
		<br>
		<h5>New personal info:</h5>
	</div>
	<br>
	<br>
	
	<?php
		/* Grab information first for the 'data_form.php' that will be soon included: */
		$customer_id = mysql_real_escape_string($_POST["cust_id"]);
		
		/* If it's not set:  */
		if(!isset($customer_id) || empty($customer_id)) header("location:index.php");
		
		/* Check if 'customer_id' is a number: */
		if(!is_numeric($customer_id))
			die("<br>ERROR: Invalid data inputted: Customer ID must be an integer<br><a href='customer_update_form.php' class='btn'>Go back</a><br><br>");
		
		/* Prepare SQL that fetches all the information of a very particular customer: */
		$sql = "SELECT p.propertyStreet, p.propertyNo, DATE_FORMAT(o.occupancyStart, '%d-%b-%y'), 
			DATE_FORMAT(o.occupancyEnd, '%d-%b-%y'), c.firstName, c.lastName
			FROM property p, occupancy o, customer c
			WHERE c.customerID = o.customerID AND o.propertyID = p.propertyID AND c.customerID = $customer_id";
		$result = mysql_query($sql) or die("Select Error: ".mysql_error()); /* Do it */
		mysql_close();

		/* Process the data: */
		$data = mysql_fetch_row($result);

		$street = $data[0];
		$street_no = $data[1];
		$start_date = $data[2];
		$end_date = $data[3];
		$first_name = $data[4];
		$last_name = $data[5];

		/* This will play a part when we include 'data_form.php' down on this code: */
		if(!isset($first_name))
			$first_name = "";
		
		/* We'll send these into the next page, which is 'customer_change_record.php': */
		$_SESSION['cust_id'] = $customer_id;
		$_SESSION['old_street_id'] = $street_no;
	?>
	
	<!-- Actually show the data form to the user: -->
	<?php include('data_form.php'); ?>
	<hr>
	<br>
</div> <!-- Body of form: END -->

<?php include("footer.php"); /* Include footer of website */ ?>