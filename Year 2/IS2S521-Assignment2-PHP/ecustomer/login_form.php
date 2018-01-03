<?php 
	session_start();
	if(isset($_SESSION["username"])) /* If he's already logged in, there's no point in logging in again */
		header("location:index.php"); /* Redirect user to the index page */
 ?>

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
    <span style='font-size:30px; color: white; text-shadow: 1px 1px #454545' class="card-title">Electricity Customers - Login</span>
      <div class="card z-depth-4">
        <div class="card-content">
		<form class="col s12" method="POST" action="login_success.php">
	        <br>
          	<div class="row">
			<div class="input-field col s5">
		  		<i class="material-icons prefix">account_circle</i>
				<input id="username" name="username" type="text" class="validate">
				<label for="username">Username</label>
			</div>
			<div class="input-field col s5">
				<input id="password" name="password" type="password" class="validate">
				<label for="password">Password</label>
			</div>
			<div class="input-field col s2">
				<input type="submit" class="btn green darken-1" value="Login">
			</div>
		</div>
          </form>
        </div>
        <div class="card-action">
		<h6>Not registered?</h6>
		<a href='register_form.php' class='btn teal darken-4'>Register</a>
        </div>
      </div>
    </div>
  </div>
</body>

<script src="//code.jquery.com/jquery-1.12.0.min.js"></script>
<script src="https://cdnjs.cloudflare.com/ajax/libs/materialize/0.97.5/js/materialize.min.js"></script>
</html>