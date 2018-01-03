/******************************************************************
Module - EC1S05 Intro to C Programming and Embedded Systems;
Course: BEng. Computer Systems Engineering Year 1;
Student No.: 140313129, Miguel Santos;

Program: Matrix Arithmetic.

Intro: This program in C interacts with the user by asking
him for two matrices and the program will output all 3
operations: addition, subtraction and multiplication
(including an extra operation: determinant).
The user will have a choice of calculating as many
times as he wants or to exit if he inputted an
invalid matrix.
*******************************************************************/

// Libraries being used:
#include <stdio.h>	// For inputting and outputting data
#include <stdlib.h>	// For exiting the program
#include <math.h>	// For calculating the matrices and determinants
#include <string.h>	// For converting ints to string and formatting
#include <conio.h>	// For 'getch()'

// GLOBAL VARIABLES:
#pragma region GLOBAL VARIABLES
/*	'pragma region' are simple macros used in Visual Studio whose purpose is to collapse code
to make it easier to read and understand. It does not affect the functionality of the program. */

const char * PROGRAM_TITLE = ">>>>>> Matrix arithmetic program <<<<<<<"; // Stores the title of the program
#define INF -pow(2,64)	// Infinity constant for returning an invalid determinant
#define MAX_SIZE 50 // The maximum size to be used for the rows and columns
#define MATRIX_AMOUNT 2 // How many matrices will be used. In this program 2 matrices will be calculated
int cols[MATRIX_AMOUNT], rows[MATRIX_AMOUNT]; // This stores the rows and columns of each matrix to be calculated
#define A 0 // Index of matrix A for indexing the rows and columns array
#define B 1 // Index of matrix B for indexing the rows and columns array 

char int2str_buffer[10];	// Global buffer that stores integers that were converted to strings
char force_continue_addsub;	// In case the user types an invalid matrix for addition but valid for multiplication so he can force the program to continue 
char force_continue_mul;	// In case the user types an invalid matrix for multiplication but valid for addition/subtraction so he can force the program to continue 
// This macro defines the end of the last defined macro. In this case, the region 'GLOBAL VARIABLE' finishes here
#pragma endregion 

// ERROR MESSAGE DATA:
#pragma region ERROR MESSAGE DATA
// Indices of the error messages for the 2D string array:
#define ADDSUB_ERROR 0			 // Index for the 'Addition/Subtraction' error string
#define MULT_ERROR 1			 // Index for 'Multiplication' error string
#define INPUT_ERROR 2			 // Index for 'Input' error string
#define SQUARE_ERROR 3			 // Index for 'invalid square matrix' error string
#define INVALID_SIZE_ERROR 4	 // Index for 'invalid matrix size' error string
#define INVALID_SIZE_MAX_ERROR 5 // Index for 'invlaid matrix max size' error string
#define ANYKEY_TOEXIT 6			 // Index for 'asking the user to press the keyboard' string
#define STILL_CONTINUE 7		 // Index for 'asking the user if he wants to stay in the program' string
const char *ERROR_MESSAGES[] = {
	{ "\n!!!!!!!!!!!!!!!!\nERROR: The addition/subtraction cannot be done. The dimensions of the matrices are not compatible.\n" },
	{ "\n!!!!!!!!!!!!!!!!\nERROR: The multiplication cannot be done. The dimensions of the matrices are not compatible.\n" },
	{ "\n!!!!!!!!!!!!!!!!\nERROR: Input is invalid! You must enter a 'double' number.\n" },
	{ "\n!!!!!!!!!!!!!!!!\nERROR: The matrix %s is not square. You must enter a N x N size matrix.\n\n" },
	{ "\n!!!!!!!!!!!!!!!!\nERROR: Cannot allocate matrices with size <=0. Please provide a positive number.\n" }, 
	{ "\n!!!!!!!!!!!!!!!!\nERROR: Cannot allocate matrices with dimensions > 50. Please provide a number between 1 and 50.\n" },
	{ "\nPress any key to exit . . . " },
	{ "\nDo you still want to continue?\n(Yes - 1 or any number | No = 0): " } };
#pragma endregion

// FUNCTION PROTOTYPES:
#pragma region FUNCTION PROTOTYPES
double** add(double ** mA, double ** mB, int rowA, int colA, int rowB, int colB); // Adds two matrices together
double** subtract(double ** mA, double ** mB, int rowA, int colA, int rowB, int colB); // Subtracts two matrices together
double** multiply(double ** mA, double ** mB, int rowA, int colA, int rowB, int colB); // Multiplies two matrices together
double   det(double ** mA, int row, int col, char * id); // Finds the determinant of a square matrix
char are_dim_valid_addition(int rowA, int colA, int rowB, int colB); // Checks if two matrices can be added or subtracted
char are_dim_valid_multi(int colA, int rowB); // Checks if two matrices can be multiplied
double** alloc_matrix(int row, int col); // Allocates a 2D array with malloc using 'row' and 'col' sizes as inputs
void   cleanup(double ** arr, int row); // Deallocates the 2D arrays that were allocated on the function 'alloc_matrix(...)'
char * int_to_string(int n); // Converts an integer to string. Useful for the formatting of strings for the user interaction
double input_double(char * message); // Allows the user to input a double number by asking him for data using a string
void   input_dimensions(); // Handles the inputting of dimensions for the matrices A and B
void   input_matrix(double ** matrix, int rows, int cols, char * matrix_id); // Allows input of actual data on any matrix provided on the parameters
void   print_matrix(double ** matrix, int rows, int cols, char * matrix_id); // Prints a matrix that is provided on the parameters
void   show_results(double ** a, double ** b, double ** c1, double **c2, double ** c3); // Outputs all results for the user
void   close_program(char * msg); // Closes the program. Useful for when there's an error while the user uses the program.
void   print_error(char * msg); // Same as 'close_program(char * msg)' except this function doesn't force the program to close
#pragma endregion

// MAIN FUNCTION:
void main(){
	// Declare matrix A, B and C1 = A + B, C2 = A-B and C3 = A * B:
	double ** a, ** b, ** c1, **c2, **c3;
	char again = 1; // This variable keeps the program working until the user chooses to exit

	// Program introduction to the user:
	printf(PROGRAM_TITLE);

	// Loop until the user chooses to close the program:
	while (again>0){
		force_continue_addsub = 0; // Reset this variable in case the user wants to recalculate matrices
		force_continue_mul = 0; // Reset this variable in case the user wants to recalculate matrices

		/*	Ask for sizes of the two matrices.
		If the rows and columns do not match in both matrices, then the user won't be
		adding/subtracting the matrices. He's given a choice to multiply the matrices.
		If this is the case, then C1 and C2 shall not be allocated and used. The same
		rule applies when the user is only able to multiply and not add/subtract. */
		input_dimensions();

		// Allocate the two matrices:
		a = alloc_matrix(rows[A], cols[A]);
		b = alloc_matrix(rows[B], cols[B]);

		// Ask for actual data:
		input_matrix(a, rows[A], cols[A], "A");
		input_matrix(b, rows[B], cols[B], "B");

		// Execute calculations:
		if (!force_continue_addsub){
			c1 = add(a, b, rows[A], cols[A], rows[B], cols[B]);
			c2 = subtract(a, b, rows[A], cols[A], rows[B], cols[B]);
		} // If the user is just multiplying don't add/subtract, so the if would be 'false'

		/*	At this point the user either inputted matrices with sizes that match or
		chose to stay at the program so he could only multiply.  */

		if (!force_continue_mul) // If this is true then the user forced the program to continue or entered a valid matrix:
			c3 = multiply(a, b, rows[A], cols[A], rows[B], cols[B]);

		// Show results:
		if (!force_continue_addsub){ // Did the user enter two matrices that can be added/subtracted?
			// Yes, he did. Show the output for those two operations:
			if (!force_continue_mul) // Check if the user added, subtracted AND multiplied
				show_results(a, b, c1, c2, c3);
			else // Else the user just added and subtracted:
				show_results(a, b, c1, c2, NULL);
		}
		else // No, he just multiplied. Don't show C1 and C2:
			show_results(a, b, NULL, NULL, c3);

		// Cleanup:
		cleanup(a, rows[A]);
		cleanup(b, rows[B]);
		if (!force_continue_addsub){ // If the user is just multiplying then 'c1' and 'c2' weren't allocated in the first place
			cleanup(c1, rows[A]);
			cleanup(c2, rows[A]);
		}
		if (!force_continue_mul) // If the user is just adding/subtracting then 'c3' wasn't allocated in the first place
			cleanup(c3, rows[A]);


		// Ask the user if he wants to calculate new matrices:
		again = (char)input_double("\n\n> Do you want to enter new matrices?\n(Yes - 1 or any number | No - 0): ");
	}
	// Program's finished executing:
	printf(ERROR_MESSAGES[ANYKEY_TOEXIT]);
	getch();
}

// MATRIX ARITHMETIC:
#pragma region MATRIX ARITHMETIC
double** add(double ** mA, double ** mB, int rowA, int colA, int rowB, int colB){
	int i, j; // Indices to be used on the looping
	double ** result; // Will store the result temporarily and its address will be returned

	// Check if the size of both matrices are valid for the operation:
	if (!are_dim_valid_addition(rowA, colA, rowB, colB))
		close_program(ERROR_MESSAGES[ADDSUB_ERROR]);

	// They are. Allocate space for the result:
	result = alloc_matrix(rowA, colB);

	// Perform addition:
	/* Algorithm:
	| x1 y1 | + | x2 y2 | = | x1+x2 y1+y2 |
	| z1 k1 |   | z2 k2 |	| z1+z2 k1+k2 |
	*/
	for (i = 0; i < rowA; i++){
		for (j = 0; j < colB; j++){
			result[i][j] = mA[i][j] + mB[i][j];
		}
	}
	return result;
}

double** subtract(double ** mA, double ** mB, int rowA, int colA, int rowB, int colB) {
	int i, j; // Indices to be used on the looping
	double ** result; // Will store the result temporarily and its address will be returned

	// Check if the size of both matrices are valid for the operation:
	if (!are_dim_valid_addition(rowA, colA, rowB, colB))
		close_program(ERROR_MESSAGES[ADDSUB_ERROR]);

	// They are. Allocate space for the result:
	result = alloc_matrix(rowA, colB);

	// Perform subtraction:
	/* Algorithm:
	| x1 y1 | - | x2 y2 | = | x1-x2 y1-y2 |
	| z1 k1 |   | z2 k2 |	| z1-z2 k1-k2 |
	*/
	for (i = 0; i < rowA; i++){
		for (j = 0; j < colB; j++){
			result[i][j] = mA[i][j] - mB[i][j];
		}
	}
	return result;
}

double** multiply(double ** mA, double ** mB, int rowA, int colA, int rowB, int colB) {
	int i, j, k; // Indices to be used on the looping
	double **result; // Will store the result temporarily and its address will be returned

	// Check if the size of both matrices are valid for the operation:
	if (!are_dim_valid_multi(colA, rowB))
		close_program(ERROR_MESSAGES[MULT_ERROR]);

	// They are. Allocate space for the result:
	result = alloc_matrix(rowA, colB);



	// Perform multiplication:
	/* Algorithm:
	| x1 y1 | * | x2 y2 | = | (x1*x2)+(y1*z2) (x1*y2)+(y1*k2) |
	| z1 k1 |   | z2 k2 |	| (z1*x2)+(k1*z2) (z1*y2)+(k1*k2) |
	*/
	for (i = 0; i < rowA; i++){
		for (j = 0; j < colB; j++){
			for (k = 0; k < rowB; k++){
				result[i][j] += mA[i][k] * mB[k][j];
			}
		}
	}
	// Multiplication done:
	return result;
}

double det(double ** mA, int row, int col, char * id){ // id is optional. Useful for debugging and for the user interface
	int square, // Stores the N from N x N
		i, j, k, k2; // Indices for looping through the matrix to find the determinant
	double ** sub_matrix, // Used to find a determinant of N x N bigger than 2 x 2 
		deter = 0.0; // Stores the result, which is the determinant value

	// Check for valid matrix before calculating determinant:
	if (row != col){ // If the row and column count are different then the matrix is not square:
		printf(ERROR_MESSAGES[SQUARE_ERROR], id);
		return INF; // Return infinity number, meaning it's an invalid determinant and the matrix is not square
	}
	if (row*col == 1) // Check if the matrix is 1x1:
		return mA[0][0]; // It is 1x1. Return the only element on the matrix

	square = row; // row is equal to col, therefore square=N from N x N

	// Matrix is valid at this point, find determinant:

	/* Find determinant for 2x2:
	Algorithm:
	det | x y | = x*k - z*y
	| z k |
	*/
	if (square == 2)
		deter = mA[0][0] * mA[1][1] - mA[0][1] * mA[1][0];
	else{
		// Find determinant for N x N using recursion:
		/* Algorithm:
		Create sub matrices (minors) from a bigger matrix till we find a 2x2 matrix. As soon as we
		get this 2x2 matrix we can easily compute the determinant. Repeat this method
		and accumulate the determinants until we find the total determinant of the original
		matrix. Recursion will be used for this algorithm.
		*/
		sub_matrix = alloc_matrix(square - 1, square - 1); // Allocate sub matrix with size square-1
		for (i = 0; i < square; i++){
			// Fill the sub_matrix it with data from the bigger matrix:
			for (j = 1; j < square; j++){
				for (k = 0, k2 = 0; k2 < square; k2++){
					if (k2 == i) continue;
					else{
						sub_matrix[j - 1][k] = mA[j][k2];
						k++;
					}
				}
			}

			// After it's filled with the data, use it to find the original determinant:
			deter += det(sub_matrix, square - 1, square - 1, id) * pow(-1, 2 + i)*mA[0][i];
		}
		// And cleanup the sub_matrix because it won't be used again:
		cleanup(sub_matrix, square - 1);
	}
	return deter;
}
#pragma endregion

#pragma region VALIDATOR FUNCTIONS
char are_dim_valid_addition(int rowA, int colA, int rowB, int colB){
	// Returns if the dimensions of both matrices are equal
	// It's used for checking if the two matrices can be added together
	return rowA == rowB && colA == colB;
}



char are_dim_valid_multi(int colA, int rowB){
	// Returns if the column of matrix 'A' is equal to the row of matrix 'B'
	// It's used for checking if two matrices can be multiplied together
	return rowB == colA;
}
#pragma endregion

#pragma region MEMORY MANAGEMENT FUNCTIONS
double** alloc_matrix(int row, int col){
	int i, j;
	double ** new_matrix; // This matrix will be allocated and its address will be returned.

	// Cannot allocate matrices with size <=0:
	if (row <= 0 || col <= 0)
		close_program(ERROR_MESSAGES[INVALID_SIZE_ERROR]);
	
	// Allocate matrix with 'row' number of rows:
	new_matrix = (double **)malloc(sizeof(double*)*row); // Cast malloc's return type from void * to double **

	// For each row, allocate 'col' number of columns:
	for (i = 0; i < row; i++){
		new_matrix[i] = (double *)malloc(sizeof(double)*col); // Cast malloc's return type from void * to double *
		// And initialize the new 2D array that was just allocated. Fill it with 0's:
		for (j = 0; j < col; j++)
			new_matrix[i][j] = 0;
	}
	// Return the address of newly allocated matrix:
	return new_matrix;
}

void cleanup(double ** arr, int row){
	int i;
	// Use function 'free(pointer)' to release the allocated memory back to the heap:
	for (i = 0; i < row; i++)
		free(arr[i]); // Free all columns from each row
	free(arr); // And finally free all rows
}
#pragma endregion

#pragma region UTILITY FUNCTIONS (MISC)
char * int_to_string(int n){
	// sprintf(buffer,format,int) takes an integer, converts it into a string and copies it to
	// a variable called 'buffer' with the format 'format'
	sprintf(int2str_buffer, "%d", n);
	return int2str_buffer;
}

double input_double(char * message){
	double input; // Variable to hold the user's input in the 'scanf'
	printf(message); // Show the message to the user asking for input	

	// Input data:
	if (!scanf_s("%lf", &input)) // Input data and check for invalid input at the 'same time'
		close_program(ERROR_MESSAGES[INPUT_ERROR]); // If this happens then the user entered an invalid input

	//At this point the user entered a valid number and the function will return the user's data carrier 'input'
	return input;
}
#pragma endregion

#pragma region INPUTTING FUNCTIONS
void input_dimensions(){
	// Ask for dimensions for the matrices and do it with a formatted output:
	printf("\n\nInputting dimensions:\n");

	// Ask for matrix A size:
	printf("> Matrix A:\n");
	rows[A] = (int)input_double("  - Row count: ");
	// Validate the size inputted:
	if (rows[A] <= 0) close_program(ERROR_MESSAGES[INVALID_SIZE_ERROR]);
	else if (rows[A] > MAX_SIZE) close_program(ERROR_MESSAGES[INVALID_SIZE_MAX_ERROR]);

	cols[A] = (int)input_double("  - Column count: ");
	// Validate the size inputted:
	if (cols[A] <= 0) close_program(ERROR_MESSAGES[INVALID_SIZE_ERROR]);
	else if (cols[A] > MAX_SIZE) close_program(ERROR_MESSAGES[INVALID_SIZE_MAX_ERROR]);

	// Ask for matrix B size:
	printf("\n> Matrix B:\n");
	rows[B] = (int)input_double("  - Row count: ");
	// Validate the size inputted:
	if (rows[B] <= 0) close_program(ERROR_MESSAGES[INVALID_SIZE_ERROR]);
	else if (rows[B] > MAX_SIZE) close_program(ERROR_MESSAGES[INVALID_SIZE_MAX_ERROR]);

	cols[B] = (int)input_double("  - Column count: ");
	// Validate the size inputted:
	if (cols[B] <= 0) close_program(ERROR_MESSAGES[INVALID_SIZE_ERROR]);
	else if (cols[B] > MAX_SIZE) close_program(ERROR_MESSAGES[INVALID_SIZE_MAX_ERROR]);

	// Check if the size of both matrices are valid for the addition/subtraction:
	if (!are_dim_valid_addition(rows[A], cols[A], rows[B], cols[B])){
		// Ask the user if he wants to continue on the program. By doing this he's still going to be multiplying:
		print_error(ERROR_MESSAGES[ADDSUB_ERROR]);
		if (input_double(ERROR_MESSAGES[STILL_CONTINUE]) <= 0) close_program("");
		force_continue_addsub = 1; // This means the program was forced to continue and this variable will be needed after this function
	}
	// Check if the size of both matrices are valid for the multiplication:
	if (!are_dim_valid_multi(cols[A], rows[B]))
	if (force_continue_addsub) // If this happens it means the matrices cannot be added/subtracted:
		close_program(ERROR_MESSAGES[MULT_ERROR]); // Don't ask the user if he wants to continue, since there is nothing else to do on the program
	else{ // Else they match their sizes and can be added/subtracted but not multiplied:
		print_error(ERROR_MESSAGES[MULT_ERROR]);
		if (input_double(ERROR_MESSAGES[STILL_CONTINUE]) <= 0) close_program("");
		force_continue_mul = 1; // This means the program was forced to continue and this variable will be needed after this function
	}
}

void input_matrix(double ** matrix, int rows, int cols, char * matrix_id){
	int i, j;
	char input_msg[20];
	printf("\n********\nInputting Matrix %s:\n", matrix_id);
	for (i = 0; i < rows; i++){
		for (j = 0; j < cols; j++){
			// Format output first by using strcpy(buffer,string) and strcat(buffer,string):
			strcpy_s(input_msg, 10, "(");

			// Format output for when there's one or more rows:
			if (rows>1)
				strcat_s(input_msg, 10, int_to_string(i));
			else
				strcat_s(input_msg, 10, "X");

			strcat_s(input_msg, 10, ","); // Insert comma between rows and columns

			// Format output for when there's one or more columns:
			if (cols > 1)
				strcat_s(input_msg, 10, int_to_string(j));
			else
				strcat_s(input_msg, 10, "X");

			strcat_s(input_msg, 10, "): ");
			// And finally ask for input with the formatted output:
			matrix[i][j] = input_double(input_msg);
		}
	}
}
#pragma endregion

#pragma region OUTPUTTING FUNCTIONS
void print_matrix(double ** matrix, int rows, int cols, char * matrix_id){
	//  Print the matrix to ease debugging and data display:
	int i, j;
	printf("\n-> Matrix: %s\n", matrix_id);
	for (i = 0; i < rows; i++){
		printf("| ");
		for (j = 0; j < cols; j++){
			if (j < cols - 1)
				printf("%.2lf ", matrix[i][j]);
			else
				printf("%.2lf |", matrix[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}


void show_results(double ** a, double ** b, double ** c1, double **c2, double ** c3){
	printf("\n____________________________\n>>>>>>>>> RESULTS >>>>>>>>>>\n");
	printf("******* Matrices:\n");
	print_matrix(a, rows[A], cols[A], "A");
	print_matrix(b, rows[B], cols[B], "B");
	if (c1 && c2){ // If the user is just multipliying the addition and subtraction weren't calculated
		print_matrix(c1, rows[A], cols[B], "C1 = A + B (Matrix addition)");
		print_matrix(c2, rows[A], cols[B], "C2 = A - B (Matrix subtraction)");
	}
	if (c3) // If the user is just adding/subtracting the multiplication was not calculated, and therefore C3 = NULL
		print_matrix(c3, rows[A], cols[B], "C3 = A * B (Matrix multiplication)");

	printf("******* Determinants:\n");
	printf("Determinant of A : %.2lf\n", det(a, rows[A], cols[A], "A"));
	printf("Determinant of B : %.2lf\n", det(b, rows[B], cols[B], "B"));
	if (c1 && c2){ // If this condition is false then the determinant of 'c1' and 'c2' should not be calculated
		printf("Determinant of C1: %.2lf\n", det(c1, rows[A], cols[B], "C1"));
		printf("Determinant of C2: %.2lf\n", det(c2, rows[A], cols[B], "C2"));
	}
	if (c3) // If this condition is false then the determinant of 'c3' should not be calculated, because C3 = NULL
		printf("Determinant of C3: %.2lf\n", det(c3, rows[A], cols[B], "C3"));
	printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n____________________________");
}
#pragma endregion

#pragma region SYSTEM FUNCTIONS
void close_program(char * msg){
	printf(msg); // Print user message before printing actual error
	printf(ERROR_MESSAGES[ANYKEY_TOEXIT]); // Print error
	getch(); // Wait for user key input
	exit(-1); // And after the user inputted any key the program will immediately close
}

void print_error(char * msg){
	// Same as 'close_program(char *msg)' except this function doesn't terminate the program
	printf(msg);
}
#pragma endregion
