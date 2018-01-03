SET serveroutput ON;

-----------------
-- Procedures: --
-----------------
CREATE OR REPLACE PROCEDURE check_id (
	c_id in customers.id%type
) AS BEGIN
	if c_id is null then
		raise value_error;
	end if;
END;
/

CREATE OR REPLACE PROCEDURE insert_record (
	c_id      in customers.id%type,
	c_name    in customers.name%type,
	c_age     in customers.age%type,
	c_address in customers.address%type,
	c_salary  in customers.salary%type
) AS BEGIN
	check_id(c_id);
	INSERT INTO CUSTOMERS VALUES (c_id, c_name, c_age, c_address, c_salary);
END;
/

CREATE OR REPLACE PROCEDURE update_record (
	c_id      in customers.id%type,
	c_name    in customers.name%type,
	c_age     in customers.age%type,
	c_address in customers.address%type,
	c_salary  in customers.salary%type
) AS BEGIN
	check_id(c_id);
	UPDATE CUSTOMERS SET name = c_name, age = c_age, address = c_address, salary = c_salary WHERE id = c_id;
END;
/

CREATE OR REPLACE PROCEDURE delete_record (
	c_id in customers.id%type
) AS BEGIN
	check_id(c_id);
	DELETE FROM CUSTOMERS WHERE id = c_id;
END;
/

---------------
-- Triggers: --
---------------
CREATE OR REPLACE TRIGGER display_salary_changes
BEFORE INSERT OR UPDATE OR DELETE
OF salary
ON customers
FOR EACH ROW
DECLARE
	saldiff number;
BEGIN
	saldiff := :new.salary - :old.salary;
	dbms_output.put_line('Old salary: ' || :old.salary);
	dbms_output.put_line('New salary: ' || :new.salary);
	dbms_output.put_line('Difference: ' || saldiff || chr(10));
END;
/

CREATE OR REPLACE FUNCTION get_total_customers
RETURN NUMBER IS
	total number;
BEGIN
	SELECT count(*) INTO total FROM customers;
	return total;
END; 
/

-------------------
-- Main Program: --
-------------------
DECLARE
BEGIN
	-- Show the amount of customers that exist on the database:
	dbms_output.put_line('Total customers: ' || get_total_customers);
	
	-- Test 1:
	delete_record(8);
	insert_record(8, 'Test', 20, 'Nowhere', 2000.0);
	update_record(8, 'New name', 10, 'Here', 5000);
	
	-- Test 2:
	delete_record(9);
	insert_record(9, 'Another', 20, 'There', 8000.0);
	update_record(9, 'New Another', 10, 'Yes', 7000);
END;
/