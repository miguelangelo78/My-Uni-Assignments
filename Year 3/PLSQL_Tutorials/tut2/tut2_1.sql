SET serveroutput ON;

CREATE OR REPLACE PROCEDURE insert_record (
	c_id      in customers.id%type,
	c_name    in customers.name%type,
	c_age     in customers.age%type,
	c_address in customers.address%type,
	c_salary  in customers.salary%type
) 
AS
BEGIN
	if c_id is NULL then
		raise value_error;
	end if;
	
	INSERT INTO CUSTOMERS VALUES (c_id, c_name, c_age, c_address, c_salary); 
END;
/

DECLARE
BEGIN
	insert_record(8, 'Name test', 20, 'Nowhere', 1900.0);
END;
/
