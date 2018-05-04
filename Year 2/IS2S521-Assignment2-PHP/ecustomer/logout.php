<?php 
    session_start();
    /* Destroy session variable, so the user can't log back in (only via the login page).
    This is all that's necessary to log a user out of a page. */
    unset($_SESSION["username"]);
?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd"> 
<html>
<head>
	<title>Electricity Customers</title>
	<link rel="stylesheet" type="text/css" href="styles.css" />
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
					<span style='font-size:40px' class="card-title">Account logged out</span>
					<p>You have logged out from your account</p>
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