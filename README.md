# LeanSQL

Very lean C library for working with tables in files in an SQL-like way.

---

## Usage

    #include <leansql.h>
    int main()
    {
        LeanSQL_init("some/folder");
        // Your code here
        return 0;
    }

All actions will return a `struct LeanSQL_ActionReport`, which can be used to know if the action succeeded or failed (and why). In case of a `SELECT`, the data will be in the action report.

---

### API

    void LeanSQL_init(char* directory);
Initialize the library. Create a new folder at `directory` if doesn't exist yet. All files managed by the library will be under that folder.

---

    struct LeanSQL_ActionReport LeanSQL_createTable(wchar_t* title, wchar_t** columns, unsigned int columns_count);
Acts like an SQL `CREATE TABLE`.
Creates a file with name `title`, with `columns_count` columns that are defined by `columns`.

---

    struct LeanSQL_ActionReport LeanSQL_dropTable(wchar_t* table);
Acts like an SQL `DROP TABLE`.
Deletes the file with the name `table`.

---

    struct LeanSQL_ActionReport LeanSQL_insert(wchar_t* table, wchar_t** data, unsigned int data_count);
Acts like an SQL `INSERT`.
Appends to file called `table` a row with `data_count` columns, defined by `data`.

---

    struct LeanSQL_ActionReport LeanSQL_select(wchar_t* table, wchar_t** columns, unsigned int columns_count, bool(*condition)(wchar_t*, wchar_t*));
Acts like an SQL `SELECT`.
Finds all rows in file called `table` that meet `condition`.
All matching rows will be under `result`.
If `columns` is `NULL` and `columns_count` is `0`, adds the entire row. Otherwise only the columns in `columns`.

---

    struct LeanSQL_ActionReport LeanSQL_update(wchar_t* table, wchar_t** data, wchar_t** columns, unsigned int columns_count, bool(*condition)(wchar_t*, wchar_t*));
Acts like an SQL `UPDATE`.
Finds all rows in file called `table` that meet `condition`.
If `columns` is `NULL`, the entire row is updated using `data`.
Otherwise, only columns in `columns` are updated matching with `data`.
`columns_count` is the size of both `columns` and `data`.

---

    struct LeanSQL_ActionReport LeanSQL_delete(wchar_t* table, bool(*condition)(wchar_t*, wchar_t*));
Acts like an SQL `DELETE`.
Finds all rows in file called `table` that meet `condition`.
These rows are deleted entirely from the file.

---

### Conditions

Condition functions follow the following conventions:
1. The first argument of the function is the column header
2. The second argument of the function is the cell value
3. The condition is applied to all cells in a row, and is then logically ORed.

---

### Data Structures

    struct LeanSQL_ResultSet
    {
        wchar_t*** data; // 2D matrix of strings
    	unsigned int columns;
    	unsigned int rows;
    }
>
    struct LeanSQL_ActionReport
    {
    	bool success;
    	enum LeanSQL_ErrorType error;
    	struct LeanSQL_ResultSet result;
    };

### Possible Errors

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
