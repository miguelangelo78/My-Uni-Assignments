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
						$street = mysql_real_escape_string($_POST["prop_street"]);
						$street_no = mysql_real_escape_string($_POST["prop_street_no"]);
						
						/* Check for clean inputs: */
						if(!is_numeric($street_no)) /* Is street_no a number */
							die("Invalid data inputted: street number must be integer<br><a href='customer_insert_form.php' class='btn'>Go back</a>");

						/* Fetch last row from tables Customer and Property: */
						$result = mysql_query("SELECT MAX(propertyID) FROM property");
						$rows = mysql_fetch_row($result);
						$last_prop_id = $rows[0] + 1; /* last property */
					
						/* Insert into Property: */
						
						/* Check if the property already exists */
						$result = mysql_query("SELECT count(*) as count FROM property WHERE propertyStreet = '$street'");
						$count = mysql_fetch_assoc($result);
						if($count['count'] == 0) {
							$result = mysql_query("SELECT MAX(propertyID) FROM property");
							$rows = mysql_fetch_row($result);
							$sql="INSERT INTO Property VALUES ($last_prop_id,'$street_no','$street')";
							mysql_query($sql) or die("Insert (Property) Error: ".mysql_error());
							
							?>
								<!-- Show success message: -->
								<h3>Success</h3>
								<hr>
								<p>New Property successfully added!</p>
							<?php
						} else {
							echo "Sorry. That property already exists";
						}

						/* New customer successfully inserted */
						mysql_close();
					?>
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