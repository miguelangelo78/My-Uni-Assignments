SET serveroutput ON;

DECLARE
	n1 number := &n1;
	n2 number := &n2;
BEGIN 
	if n1 > n2 then
		dbms_output.put_line('First is greater than second');
	elsif n1 < n2 then
		dbms_output.put_line('First is less than second');
	else
		dbms_output.put_line('First is the same as second');
	end if;
END;
/