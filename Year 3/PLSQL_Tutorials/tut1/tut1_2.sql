SET serveroutput ON;

DECLARE
	c_name    customers.name%type;
	c_address customers.address%type;
	c_salary  customers.salary%type;
BEGIN
	SELECT name, address, salary INTO c_name, c_address, c_salary FROM customers WHERE id = 1;
	dbms_output.put_line('Name: ' || c_name || ' Address: ' || c_address || ' Salary: ' || c_salary);
END;
/