<!--  Display the entire database contents in just this file, which should be included by other pages -->

<div class="row chip">
	<?php 
		if(isset($display_property))
			$_GET["display"] = "properties"; /* Force the display of properties */
	
		echo "Display ";
		switch($_GET["display"]) {
			case "history": echo "History"; break;
			default: case "customers": echo "Customers"; break;
			case "occupations": echo "Occupations"; break;
			case "properties": echo "Properties"; break;
		}
	?>
</div>

<div class="row">
	<div class="col s10 offset-s1">
		<!--  Show data in a table -->
		<table class="highlight centered" style="font-size:12px" id="display_table">
		<!-- Table header: -->
		<thead>
			<?php 
				/* Prepare SQL statement that displays the whole database, according
				to the assignment's requirements: */
				$sql = "";
				switch($_GET["display"]) {
				case "history": 
					$sql = "SELECT c.customerID AS 'Customer ID', 
							p.propertyStreet AS Street, 
							p.propertyNo AS No, 
							DATE_FORMAT(o.occupancyStart, '%d-%b-%y') AS Start, 
							DATE_FORMAT(o.occupancyEnd, '%d-%b-%y') AS End, 
							c.firstName AS 'First Name', 
							c.lastName AS Surname
						FROM property p, occupancy o, customer c
						WHERE c.customerID = o.customerID AND o.propertyID = p.propertyID
						ORDER by p.propertyStreet ASC, p.propertyNo ASC, o.occupancyStart ASC";
					break;
				default: /* Oops, invalid $_GET variable, we're just going to display the customers */
				case "customers": 
					$sql = "SELECT c.customerID AS 'Customer ID', 
								c.firstName AS 'First Name', 
								c.lastName AS Surname
						FROM customer c
						ORDER by c.customerID ASC";
					break;
				case "occupations": 
					$sql = "SELECT o.occupancyID AS ID, 
								o.propertyID AS 'Property ID', 
								o.customerID AS 'Customer ID', 
								DATE_FORMAT(o.occupancyStart, '%d-%b-%y') AS Start, 
								DATE_FORMAT(o.occupancyEnd, '%d-%b-%y') AS End
						FROM occupancy o
						ORDER by o.occupancyID ASC";
					break;
				case "properties": 
					$sql = "SELECT p.propertyID AS ID, 
							p.propertyNo AS No,
							p.propertyStreet AS Street
						FROM property p
						ORDER by p.propertyID ASC";
					break;
				}
			
				/* Fetch it: */
				$result = mysql_query($sql) or die("Display Error: ".mysql_error());
				
				/* Print column names: */
				for($i = 0; $i < mysql_num_fields($result); $i++) {
					$column_name = mysql_fetch_field($result, $i);
					echo "<th>{$column_name->name}</th>";
				}
			?>
			
			</tr>
		</thead> <!-- Table header: END -->

		<!-- Table body: -->
		<tbody>
			<?php				
				/* Show the retrieved data: */
				while ($get_info = mysql_fetch_row($result))
				{
					echo "<tr>\n"; /* Output a table row */
					foreach ($get_info as $field)
						if($field == NULL) /* If date is 00-00-00, we don't want to see it displayed */
							echo "<td>-</td>\n";
						else
							echo "<td>$field</td>\n"; /* Display the field/column normally */
					echo "</tr>\n"; /* Close table row */
				}
			?>
		</tbody> <!-- Table body: END -->
		</table>
	</div>
</div>