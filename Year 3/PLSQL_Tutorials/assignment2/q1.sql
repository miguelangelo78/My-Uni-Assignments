SET serveroutput ON;

-- Define procedure for adding new books into the Database:
CREATE OR REPLACE PROCEDURE addBook (
	v_barcode_id       in book_copies.barcode_id%type,
	v_isbn             in books.isbn%type,
	title              in books.title%type,
	summary            in books.summary%type,
	author             in books.author%type,
	date_published_str in varchar2,
	page_count         in books.page_count%type
)
AS
	-- Declare variables for this procedure:
	book_already_exists number := 0;
	bad_barcode         exception;
	bad_isbn            exception;
	date_published      books.date_published%type;
	is_copy             boolean := false;
BEGIN
	-- Check for bad input:
	if v_barcode_id is null then
		raise bad_barcode;
	end if;
	if v_isbn is null then
		raise bad_isbn;
	end if;

	-- Convert date from string to the actual date type:
	date_published := TO_DATE(date_published_str, 'DD-MON-YYYY', 'NLS_DATE_LANGUAGE=ENGLISH');
	
	-- Check if this book UNIQUE 'copy' already exists on the Database:
	SELECT COUNT(*) INTO book_already_exists FROM book_copies WHERE barcode_id = v_barcode_id;
	
	if book_already_exists > 0 then
		-- This book copy already exists!
		raise DUP_VAL_ON_INDEX;
	end if;
	
	-- Check if this book (not the copy) already exists on the Books table:
	SELECT COUNT(*) INTO book_already_exists FROM books WHERE isbn = v_isbn;
	
	-- Insert a new book only if it does NOT already exist (since we don't want to cause an exception):
	if book_already_exists = 0 then
		-- The book does not exist:
		INSERT INTO books (isbn, title, summary, author, date_published, page_count) 
		VALUES (v_isbn, title, summary, author, date_published, page_count);
	else
		-- The book already exists. We must still update this book in order to execute all triggers
		is_copy := true;
		UPDATE books
		SET isbn = v_isbn, title = title, summary = summary, author = author, date_published = date_published, page_count = page_count
		WHERE isbn = v_isbn;
	end if;
	
	-- Insert new Book copy (this might cause an exception):
	INSERT INTO book_copies (barcode_id, isbn)
	VALUES (v_barcode_id, v_isbn);
			
	-- Show success message:
	dbms_output.put('>> Book');
	if is_copy = true then
		dbms_output.put(' (copy)');
	end if;
	dbms_output.put_line(' '''|| title ||''' successfully added!');
EXCEPTION
	-- On duplicate book copy's barcode id:
	WHEN DUP_VAL_ON_INDEX THEN
		dbms_output.put_line('>> ERROR: Book copy '''|| title ||''' (b.code: ' || v_barcode_id || ') already exists!');
	
	-- On bad barcode input:
	WHEN bad_barcode THEN
		dbms_output.put_line('>> ERROR: Barcode ID '''|| v_barcode_id ||''' for book '''|| title ||''' is invalid!');

	-- On bad isbn input:
	WHEN bad_isbn THEN
		dbms_output.put_line('>> ERROR: ISBN '''|| v_isbn ||''' for book '''|| title ||''' is invalid!');
END;
/

-------------------
-- Main program: --
-------------------
DECLARE
BEGIN
	-- Add new books:
	addBook('100000001', '1-56592-335-9', 'Oracle PL/SQL Programming', 'Reference for PL/SQL developers, including examples and best practice recommendations.', 'Feuerstein, Steven with Bill Pribyl', '01-SEP-1997', 987);
	addBook('100000002', '1-56592-335-9', 'Oracle PL/SQL Programming', 'Reference for PL/SQL developers, including examples and best practice recommendations.', 'Feuerstein, Steven with Bill Pribyl', '01-SEP-1997', 987);
	
	addBook('100000015', '0-14071-483-9', 'The tragedy of King Richard the Third', 'Modern publication of popular Shakespeare historical play in which a treacherous royal attempts to steal the crown but dies horseless in battle.', 'William Shakespeare', '01-AUG-2000', 158);
	addBook('100000016', '0-14071-483-9', 'The tragedy of King Richard the Third', 'Modern publication of popular Shakespeare historical play in which a treacherous royal attempts to steal the crown but dies horseless in battle.', 'William Shakespeare', '01-AUG-2000', 158);
	
	addBook('100000030', '1-56592-457-6', 'Oracle PL/SQL Language Pocket Reference', 'Quick-reference on Oracle''s PL/SQL language.', 'Feuerstein, Steven with Bill Pribyl and Chip Dawes', '01-APR-1999', 94);
	addBook('100000022', '1-56592-457-6', 'Oracle PL/SQL Language Pocket Reference', 'Quick-reference on Oracle''s PL/SQL language.', 'Feuerstein, Steven with Bill Pribyl and Chip Dawes', '01-APR-1999', 94);
	addBook('100000020', '1-56592-457-6', 'Oracle PL/SQL Language Pocket Reference', 'Quick-reference on Oracle''s PL/SQL language.', 'Feuerstein, Steven with Bill Pribyl and Chip Dawes', '01-APR-1999', 94);
		
	-- Show success message:
	dbms_output.put_line('>> SUCCESS: All books added.');
END;
/