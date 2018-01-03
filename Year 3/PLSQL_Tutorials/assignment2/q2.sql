SET serveroutput ON;

DECLARE
	-- Declare count variable:
	book_count       number;
	total_book_count number;

	-- Declare procedure for fetching the amount of books present on the Database:
	PROCEDURE getBookCount (
		book_count       out number,
		total_book_count out number
	) IS BEGIN
		-- Run count(*) SQL function and store it in the 'book_count' and 'total_book_count' variables:
		SELECT COUNT(*) INTO book_count       FROM books;
		SELECT COUNT(*) INTO total_book_count FROM book_copies;
		-- No need to return values since this is a procedure
	END;

-------------------
-- Main program: --
-------------------
BEGIN
	-- Fetch the book count from the Database:
	getBookCount(book_count, total_book_count);
	-- Show it to the user:
	dbms_output.put_line('>> Number of UNIQUE books in the Database: ' || book_count || '. Total amount of books including copies: ' || total_book_count || '.');
END;
/