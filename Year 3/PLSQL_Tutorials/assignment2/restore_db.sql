DROP TABLE book_copies;
DROP TABLE books;

CREATE TABLE books (
   isbn VARCHAR2(13) NOT NULL PRIMARY KEY,
   title VARCHAR2(200),
   summary VARCHAR2(2000),
   author VARCHAR2(200),
   date_published DATE,
   page_count NUMBER
);

CREATE TABLE book_copies(
   barcode_id VARCHAR2(100) NOT NULL PRIMARY KEY,
   isbn VARCHAR2(13) REFERENCES books (isbn)
);
