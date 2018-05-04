<div class='row'> <!-- Embed form into a whole row -->
	
	<?php if(!isset($insert_property)) { ?>
	<!-- Display Customer Form: -->
	
	<!-- This form is being used by multiple pages, to prevent code redundancy: -->
	<form class="col s12" method="POST" action="<?php if(!isset($first_name)) echo 'customer_insert_record.php'; else echo 'customer_change_record.php';?>">
		<div class="row"> <!-- Customer name -->
			<div class="input-field col s6">
				<i class="material-icons prefix">account_circle</i>
				<input <?php if(isset($first_name)) echo "value='$first_name'"; ?> id="first_name" name="first_name" type="text" class="validate">
				<label for="first_name">First Name</label>
			</div>
			<div class="input-field col s6">
				<input <?php if(isset($last_name)) echo "value='$last_name'"; ?> id="last_name" name="last_name" type="text" class="validate">
				<label for="last_name">Last Name</label>
			</div>
		</div> <!-- Customer name: END -->
		<div class="row"> <!-- Customer location -->
			<div class="input-field col s6">
				<i class="material-icons prefix">location_on</i>
				<input <?php if(isset($street)) echo "value='$street'"; ?> id="street" name="street" type="text" class="validate">
				<label for="street">Street</label>
			</div>
			<div class="input-field col s6">
				<input <?php if(isset($street_no)) echo "value='$street_no'"; ?> id="street_no" name="street_no" type="text" class="validate">
				<label for="street_no">No.</label>
			</div>
		</div> <!-- Customer location: END -->
		<div class="row"> <!-- Customer dates -->
			<div class="input-field col s6">
				<i class="material-icons prefix">today</i>
				<input <?php if(isset($start_date)) echo "value='$start_date'"; ?> id="start_date" name="start_date" type="text" class="validate">
				<label for="start_date">Start Date</label>
			</div>
			<div class="input-field col s6">
				<input <?php if(isset($end_date)) echo "value='$end_date'"; ?> id="end_date" name="end_date" type="text" class="validate">
				<label for="end_date">End Date</label>
			</div>
		</div> <!-- Customer dates: END -->
		<div class="row"> <!-- Add Customer Button -->
			<div class="col s1 offset-s<?php if(!isset($first_name)) echo '8'; else echo '9'; ?>">
				<input type="submit" class="btn" value="<?php if(!isset($first_name)) echo 'Add Customer'; else echo 'Update';?>">
			</div>
		</div> <!-- Add Customer Button: END -->
	</form>
	
	<?php } else { ?>
	<!-- Display Property Form: -->
	
	<form class="col s12" method="POST" action="property_insert_record.php">
		<div class="row"> <!-- Property form: START -->
			<div class="input-field col s6">
				<i class="material-icons prefix">location_on</i>
				<input <?php if(isset($street)) echo "value='$street'"; ?> id="prop_street" name="prop_street" type="text" class="validate">
				<label for="prop_street">Property Address</label>
			</div>
			<div class="input-field col s6">
				<input <?php if(isset($street_no)) echo "value='$street_no'"; ?> id="prop_street_no" name="prop_street_no" type="text" class="validate">
				<label for="prop_street_no">Property No.</label>
			</div>
			<div class="row"> <!-- Add Property Button -->
				<div class="col s1 offset-s<?php if(!isset($first_name)) echo '8'; else echo '9'; ?>">
					<input type="submit" class="btn" value="Add Property">
				</div>
			</div> <!-- Add Property Button: END -->
		</div> <!-- Property form: END -->
	</form>
	
	<?php } ?>
<div>