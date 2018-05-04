<?php 

/* Database's sensitive data: */
$username = "YOURUSERNAME";
$password = "YOURPASSWORD";
$database = "YOURDATABASE";

/* Connect to the database right away: */
$comm = @mysql_connect(localhost, $username, $password);
$rs = @mysql_select_db($database) or die( "Unable to select database");

/* Make the hash very difficult to decrypt: */
$hash_cost = 100;

/* This function receives a password in plain text and converts it into a very
	hard to decrypt hash, which will be stored in the database as the password: */
function make_hash($password) {
	global $hash_cost;

	/* Create a new random salt: */
	$new_salt = strtr(base64_encode(mcrypt_create_iv(16, MCRYPT_DEV_URANDOM)), '+', '.');
	
	/* Format salt with Blowfish algorithm: */
	$new_salt = sprintf("$2a$%02d$", $hash_cost).$new_salt;
	
	/* Encyrpt password with salt and return it: */
	return crypt($password, $new_salt);
}

/* The function 'hash_equals' does not exist on this server's php version, 
   and we need it to check the hashed password: */
if(!function_exists('hash_equals')) {
	function hash_equals($str1, $str2) {
		if(strlen($str1) != strlen($str2)) {
			return false;
		} else {
			$res = $str1 ^ $str2;
			$ret = 0;
			for($i = strlen($res) - 1; $i >= 0; $i--) $ret |= ord($res[$i]);
			return !$ret;
		}
	}
}

function check_password($unhashed_password, $hashed_password) {
	/* Check if password is valid: */
	return hash_equals($hashed_password, crypt($unhashed_password, $hashed_password));
}

?>