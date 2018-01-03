SET serveroutput ON;
-- This program needs to be executed on SQL Developer by clicking on the 
-- button 'Run Script (F5)' and NOT by pressing the button 
-- 'Run Statement (CTRL+ENTER)' because of the user prompt

DECLARE
	-- Request user the ISBN number:
	-- !!IMPORTANT!! Make sure to type the isbn with the character ' when the program asks for user input. Example: '1-56592-335-9'
	isbn_input books.isbn%type := &isbn;

	PROCEDURE delete_book (
		v_isbn books.isbn%type,
		bypass_confirmation boolean
	) IS
		book_exists       number := 0;
		user_confirmation number := 0;
	BEGIN
		dbms_output.put('> Deleting book ''' || v_isbn || '''... ');
		
		-- Check if this book exists on the Database:
		SELECT COUNT(*) INTO book_exists FROM books WHERE isbn = v_isbn;
		if book_exists = 0 then
			dbms_output.put_line(' ERROR: This book is non-existant!');
		else
			-- The book exists, continue execution:
			if bypass_confirmation = false then
				-- Request user permission:
				user_confirmation := &are_you_sure_0no_1yes;	
			else
				-- We're bypassing the user confirmation:
				user_confirmation := 1;
			end if;
			
			if user_confirmation != 0 then
				-- Deleting book and its copies:
				DELETE FROM book_copies WHERE isbn = v_isbn;
				DELETE FROM books WHERE isbn = v_isbn;
				dbms_output.put_line(' Done');
			else
				-- User chose not to delete this book:
				dbms_output.put_line(' NOTICE: Book ''' || v_isbn || 
					''' was NOT deleted');
			end if;
		end if;
	END;

-------------------
-- Main program: --
-------------------
BEGIN
	delete_book(isbn_input, false);
END;
/
