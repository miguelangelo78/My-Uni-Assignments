<?php include("dbinfo_inc.php"); ?>

<!-- The header is not included here because the header being used on the other pages
is very different from the header of this page (on login page and register page).
Because of this, this page will get its own style and structure without including a php file. --> 

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd"> 
<html>
	<head>
		<title>Electricity Customers</title>
		<link rel="stylesheet" type="text/css" href="styles.css" />
		<link rel="shortcut icon" type="image/png" href="favicon.ico"/>
		<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/materialize/0.97.5/css/materialize.min.css">
		<link href='http://fonts.googleapis.com/css?family=Open+Sans' rel='stylesheet' type='text/css'>
		<link href="https://fonts.googleapis.com/icon?family=Material+Icons" rel="stylesheet">
		<link href='https://fonts.googleapis.com/css?family=Roboto' rel='stylesheet' type='text/css'>
	</head>

	<body class="blue-grey darken-1">
		<div id='login_form' class="row center-align">
			<div class="col s6 offset-s3">
				<div class="card blue-grey darken-2">
					<div class="card-content white-text">
						<?php
							/* Grab username from POST: */
							$username = mysql_real_escape_string($_POST['username']);
							/* Check for dirty input: */
							if(empty($username) || empty($_POST['password'])) 
								die("Invalid data inputted into the fields<br><a href='register_form.php' class='btn'>Go back</a>");
							
							/* Check if user already exists */
							$sql = "SELECT * FROM account WHERE account_username='$username'";
							$result = mysql_query($sql);
							if(mysql_num_rows($result) >= 1)
								die("User '$username' already exists<br><a href='register_form.php' class='btn'>Go back</a>");
							
							/* The user doesn't exist. Create new user: */
							
							/* Hash password first: */
							$password = make_hash(mysql_real_escape_string($_POST['password']));
							
							/* And insert it, including the username, into the database: */
							$sql = "INSERT INTO account (account_username, account_password) VALUES ('$username', '$password')";
							if(mysql_query($sql)) { 
						?>
								<span style='font-size:40px' class="card-title">Account Created</span>
								<p>You have registered successfully</p>
						<?php } else { ?>
								<span style='font-size:40px' class="card-title">Error</span>
								<p>Could not create account</p>
								<?php echo 'Register error: '. mysql_error(); 
							} 
						?>
					</div>
					<div class="card-action blue-grey darken-1">
						<a href="login_form.php" class='btn'>Go to Login Page</a>
					</div>
				</div>
			</div>
		</div>
	</body>

	<script src="//code.jquery.com/jquery-1.12.0.min.js"></script>
	<script src="https://cdnjs.cloudflare.com/ajax/libs/materialize/0.97.5/js/materialize.min.js"></script>
</html>