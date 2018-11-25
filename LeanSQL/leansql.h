#pragma once

#include <wchar.h>
#include <stdbool.h>

struct LeanSQL_ResultSet
{
	wchar_t*** data; // 2D matrix of strings
	unsigned int columns;
	unsigned int rows;
	// Note about memory leaks:
	/* `data` is owned by you.
	 * You'll have to call `free()` on all the strings,
	 * all the rows, and `data` itself.
	 * There's a helper function for that below: `LeanSQL_destroyResultSet`
	 */
};

void LeanSQL_destroyResultSet(struct LeanSQL_ResultSet*);

// All the possible errors
enum LeanSQL_ErrorType
{
	LEANSQL_ERROR_PARAMETER, // function given invalid parameter
	LEANSQL_ERROR_MEMORY, // failed malloc
	LEANSQL_ERROR_IO, // error reading or writing file
	LEANSQL_ERROR_NO_TABLE, // operation on non-existant table
	LEANSQL_ERROR_TABLE_MISMATCH, // table does not support operation (for example: incorrect number of columns)
	LEANSQL_ERROR_TABLE_MALFORMED, // table is not rectangular
	LEANSQL_ERROR_UNKNOWN
};

// Will be returned by all actions
struct LeanSQL_ActionReport
{
	bool success; // did the action succeed?
	enum LeanSQL_ErrorType error; // if it didn't succeed, why?
	struct LeanSQL_ResultSet result; // if it succeeded, what data does it yield?
};

// Call this first. It creates a directory for the database if it doesn't exist yet.
void LeanSQL_init(char* directory);

// SQL: CREATE TABLE
// Creates a file
struct LeanSQL_ActionReport LeanSQL_createTable(wchar_t* title, wchar_t** columns, unsigned int columns_count);

// SQL: DROP TABLE
// Deletes a file
struct LeanSQL_ActionReport LeanSQL_dropTable(wchar_t* table);

// SQL: INSERT
// Appends to file
struct LeanSQL_ActionReport LeanSQL_insert(wchar_t* table, wchar_t** data, unsigned int data_count);

// SQL: SELECT
// Reads from file
struct LeanSQL_ActionReport LeanSQL_select(wchar_t* table, wchar_t** columns, unsigned int columns_count, bool(*condition)(wchar_t*, wchar_t*, void*), void* extra_data);

// SQL: Update
// Finds in file matching `condition`, and update `columns` using `data`.
struct LeanSQL_ActionReport LeanSQL_update(wchar_t* table, wchar_t** data, wchar_t** columns, unsigned int columns_count, bool(*condition)(wchar_t*, wchar_t*, void*), void* extra_data);

// SQL: Delete
// Finds in file matching `condition`, and deletes the entire row.
struct LeanSQL_ActionReport LeanSQL_delete(wchar_t* table, bool(*condition)(wchar_t*, wchar_t*, void*), void* extra_data);
