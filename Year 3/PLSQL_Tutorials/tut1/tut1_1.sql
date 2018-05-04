set serveroutput on
DECLARE
	msg varchar2(20) := 'Hello World';
BEGIN
	dbms_output.put_line(msg);
END;
/