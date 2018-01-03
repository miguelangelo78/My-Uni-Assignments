SET serveroutput ON;

DECLARE
	-------------------------------------------
	----------------- Q4 ----------------------
	-------------------------------------------
	-- These variables will be used only for the 4th question on the assignment:
	CURSOR book_cur IS
		SELECT isbn FROM books;
	
	book_iterator book_cur%rowtype;
	counter number := 1; -- This variable will be used for output purposes only
	-------------------------------------------
	------------ Q4: END ----------------------
	-------------------------------------------

	-------------------------------------------
	----------------- Q3 ----------------------
	-------------------------------------------
	-- Create new custom type in order to store different types of variables:
	TYPE book_details_t IS RECORD (
		isbn           books.isbn%type,
		title          books.title%type,
		author         books.author%type,
		date_published books.date_published%type,
		copy_count     number,
		is_valid       boolean -- This boolean will only indicate if the record has valid data inside
	);
	
	-- Declare function which retrieves the book's details:
	FUNCTION getBookDetails (
		v_isbn books.isbn%type
	) RETURN book_details_t
	IS
		data_fetched book_details_t;
	BEGIN
		-- We're storing the ISBN so we can identify this record later even if it does not exist on the Database:
		data_fetched.isbn := v_isbn;
		
		-- Fetch book details:
		SELECT title, author, date_published 
		INTO data_fetched.title, data_fetched.author, data_fetched.date_published
		FROM books
		WHERE isbn = v_isbn;
		
		-- Fetch the number of book copies for this given book:
		SELECT COUNT(*) 
		INTO data_fetched.copy_count 
		FROM book_copies 
		WHERE isbn = v_isbn;
		
		-- The data is good to be used:
		data_fetched.is_valid := true;
		
		-- Return the data:
		return data_fetched;
	EXCEPTION
		-- Return an invalid record:
		WHEN no_data_found THEN
			data_fetched.is_valid := false;
			data_fetched.title    := '<NIL>';
			data_fetched.author   := '<NIL>';
			data_fetched.date_published := TO_DATE('01-JAN-9999', 'DD-MON-YYYY', 'NLS_DATE_LANGUAGE=ENGLISH');
			data_fetched.copy_count     := -1;
			return data_fetched;
	END;
	------------------------------------------------
	----------------- Q3: END ----------------------
	------------------------------------------------
	
	-------------------------------------------
	----------------- Q4 ----------------------
	-------------------------------------------
	-- Declare procedure that shows the details of a given book:
	PROCEDURE report_details (
		details in book_details_t
	) IS BEGIN
		if details.is_valid = true then
			dbms_output.put_line('- Title: ''' || details.title 
				|| ''', Author: ''' || details.author 
				|| ''', Date Pub.: ''' || TO_CHAR(details.date_published, 'DD-MON-YYYY', 'NLS_DATE_LANGUAGE=ENGLISH')
				|| ''', Copies: ' || details.copy_count || chr(10));
		else
			dbms_output.put_line('>> ERROR: The book with ISBN '''||details.isbn||''' does not exist!'||chr(10));
		end if;
	END;

-------------------
-- Main program: --
-------------------
BEGIN
	-- Open cursor in order to start iterating through all the books:
	OPEN book_cur;
	
	-- Start iterating:
	LOOP
		-- Grab record:
		FETCH book_cur into book_iterator;
		EXIT WHEN book_cur%notfound; -- Exit the loop if there are no more records
		
		-- And show the data to the user:
		dbms_output.put('> ' || counter);
		counter := counter + 1;
		report_details(getBookDetails(book_iterator.isbn)); -- Cascading functions inside functions' parameters may help code readability in some cases
	END LOOP;
	
	-- And finally, close the cursor:
	CLOSE book_cur;
END;
/

-------------------------------------------
------------ Q4: END ----------------------
-------------------------------------------