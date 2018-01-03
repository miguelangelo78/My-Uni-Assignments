SET serveroutput ON;
-- NOTE: This program only works if the procedure 'addBook' from question 1 was previously
-- created and exists on the database

-- Create the trigger for displaying the book copy 
-- count every time the books table changes:
CREATE OR REPLACE TRIGGER display_book_copies_on_changes
AFTER INSERT OR UPDATE OR DELETE -- This trigger will execute on every insert, update and delete
ON books -- On this table
FOR EACH ROW -- Every time any row changes
DECLARE
	copy_count number;
BEGIN
	-- If :new.isbn is null, then a deletion must've happened
	if :new.isbn is null then
		SELECT COUNT(*) INTO copy_count FROM book_copies WHERE isbn = :old.isbn;
		dbms_output.put_line('>> TRIGGER (DELETION): The book ''' || :old.title || ''' (''' || :old.isbn || ''') has now ' || copy_count || ' copies.' || chr(10));
	else
		-- If not, then we're inserting/updating the table books:
		SELECT COUNT(*) INTO copy_count FROM book_copies WHERE isbn = :new.isbn;
		copy_count := copy_count + 1; -- We need to increment this by one because we ALWAYS insert into the books table before we insert into the book_copies
		dbms_output.put_line(chr(10) || '>> TRIGGER (INSERTION/UPDATE): The book ''' || :new.title || ''' (''' || :new.isbn || ''') has now ' || copy_count || ' copies.');
	end if;
END;
/

-------------------
-- Main program: --
-------------------
DECLARE BEGIN
	-- Clear Database first:
	dbms_output.put_line('********************************');
	dbms_output.put_line('******** Deleting books ********');
	dbms_output.put_line('********************************');
	DELETE FROM book_copies;
	DELETE FROM books;

	-- Add the following books again in order to visualise the triggers being executed:
	dbms_output.put_line('******************************');
	dbms_output.put_line('******** Adding books ********');
	dbms_output.put_line('******************************');
	addBook('100000001', '1-56592-335-9', 'Oracle PL/SQL Programming', 'Reference for PL/SQL developers, including examples and best practice recommendations.', 'Feuerstein, Steven with Bill Pribyl', '01-SEP-1997', 987);
	addBook('100000002', '1-56592-335-9', 'Oracle PL/SQL Programming', 'Reference for PL/SQL developers, including examples and best practice recommendations.', 'Feuerstein, Steven with Bill Pribyl', '01-SEP-1997', 987);
	
	addBook('100000015', '0-14071-483-9', 'The tragedy of King Richard the Third', 'Modern publication of popular Shakespeare historical play in which a treacherous royal attempts to steal the crown but dies horseless in battle.', 'William Shakespeare', '01-AUG-2000', 158);
	addBook('100000016', '0-14071-483-9', 'The tragedy of King Richard the Third', 'Modern publication of popular Shakespeare historical play in which a treacherous royal attempts to steal the crown but dies horseless in battle.', 'William Shakespeare', '01-AUG-2000', 158);
	
	addBook('100000030', '1-56592-457-6', 'Oracle PL/SQL Language Pocket Reference', 'Quick-reference on Oracle''s PL/SQL language.', 'Feuerstein, Steven with Bill Pribyl and Chip Dawes', '01-APR-1999', 94);
	addBook('100000022', '1-56592-457-6', 'Oracle PL/SQL Language Pocket Reference', 'Quick-reference on Oracle''s PL/SQL language.', 'Feuerstein, Steven with Bill Pribyl and Chip Dawes', '01-APR-1999', 94);
	addBook('100000020', '1-56592-457-6', 'Oracle PL/SQL Language Pocket Reference', 'Quick-reference on Oracle''s PL/SQL language.', 'Feuerstein, Steven with Bill Pribyl and Chip Dawes', '01-APR-1999', 94);
END;
/

DROP TRIGGER display_book_copies_on_changes;
