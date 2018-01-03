<?php 
	session_start();
	if(!isset($_SESSION["username"])) /* If it's set, nothing happens and the user continues forward */
		header("location:login_form.php"); /* Otherwise, redirect him to the login page */
?>