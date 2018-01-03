<?php 
	session_start();
	if(!isset($_POST["username"])) /* If the user is not logged in, then prevent him from getting in without logging in */
		header("location:logout.php"); /* Keep him out of the index page */
	/* Otherwise, the user did indeed try to login */
	include("dbinfo_inc.php");
?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
"http://www.w3.org/TR/html4/loose.dtd"> 
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
				$success = 0;
				/* Fetch post data: */
				$username = mysql_real_escape_string($_POST['username']);
				$password = mysql_real_escape_string($_POST['password']);
				/* Protect page against dirty input: */
				if(empty($username) || empty($password))
					die("Invalid data inputted into the fields<br><a href='login_form.php' class='btn'>Go back</a>");
				
				/* Prepare SQL statement for fetching a single username: */
				$sql = "SELECT * FROM account WHERE account_username='$username'";
				$result = mysql_query($sql); /* Do it */
				if($result and mysql_num_rows($result) == 1) { /* It's only valid if the result returned 1 row */
					$row = mysql_fetch_assoc($result); /* Grab row data */
					if(check_password($password, $row["account_password"])) /* And check if password is valid */
						$success = 1; /* It is */
					/* Otherwise it isn't, and 'success', will stay at 0 */
				}
				
				if($success) { /* If successful */
					$_SESSION["username"] = $username;
				?>
					<!-- NOTE: This html code is being 'echoed' by PHP. See the previous 'if' a couple lines above. -->
					<span style='font-size:40px' class="card-title">Login Success</span>
					<p>You have logged in successfully</p>
			<?php } else { ?>
				<!-- NOTE: This html code is being 'echoed' by PHP. See the previous 'if' a couple lines above. -->
				<span style='font-size:40px' class="card-title">Login Unsuccessful</span>
				<p>Wrong username and/or password</p>
				<?php echo mysql_error(); 
			} ?>
			</div>
			<div class="card-action blue-grey darken-1">
			<?php if($success) { ?>
				<a href="index.php" class='btn'>Go to Homepage</a>
			<?php } else { ?>
				<a href="login_form.php" class='btn'>Go to Login Page</a>
			<?php } ?>
			</div>
          	</div>
        </div>
	</div>
</body>

<script src="//code.jquery.com/jquery-1.12.0.min.js"></script>
<script src="https://cdnjs.cloudflare.com/ajax/libs/materialize/0.97.5/js/materialize.min.js"></script>
</html>