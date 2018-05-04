SET serveroutput ON;

DECLARE 
	ctr number := &ctr;
BEGIN 
	for i in 0..ctr-1 loop
		dbms_output.put_line('Hello world -> ' || i);
	end loop;
END;
/