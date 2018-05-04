<?php
	/* Include essentials */
	include("is_logged.php"); /* Checks if it's logged in, and redirects if it's not */
	include("header.php"); /* Loads page header */
	/* We don't include the database info in the index page because we're not going to
	do any SQL queries to the database here. */
?>

<!-- Present the user with a navigation bar an image: -->
<div class="card">    		
<div class="card-action" style='margin-bottom: -25px'>
	<div style='margin-bottom: 15px'>
	<a href="customer_insert_form.php" class="waves-effect waves-yellow index_btn"><i class="material-icons">library_add</i>  Insert</a>
	<a href="customer_update_form.php" class="waves-effect waves-yellow index_btn"><i class="material-icons">loop</i>  Update</a>
	<a href="customer_delete_form.php" class="waves-effect waves-yellow index_btn"><i class="material-icons">delete</i>  Delete</a>
	</div>
	<hr>
	<div class="row chip">
		Display:
	</div>
	<a style="font-size:13px" href="display_page.php?display=history" class="waves-effect waves-yellow index_btn">History Page</a>
	<a style="font-size:13px" href="display_page.php?display=customers" class="waves-effect waves-yellow index_btn">Customers</a>
	<a style="font-size:13px" href="display_page.php?display=occupations" class="waves-effect waves-yellow index_btn">Occupations</a>
	<a style="font-size:13px" href="display_page.php?display=properties" class="waves-effect waves-yellow index_btn">Properties</a>
</div>
<div class="card-image">
	<img src="https://upload.wikimedia.org/wikipedia/commons/d/d9/Arduino_ftdi_chip-1.jpg">
</div>
<div class="card-content">
	<p>Welcome to the Electricity Customer Database Application</p>
</div>
</div>

<?php include("footer.php"); /* Include footer of website */ ?>