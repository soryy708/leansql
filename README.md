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

    struct LeanSQL_ActionReport LeanSQL_select(wchar_t* table, wchar_t** columns, unsigned int columns_count, bool(*condition)(wchar_t*, wchar_t*, void*), void* extra_data);
Acts like an SQL `SELECT`.
Finds all rows in file called `table` that meet `condition`.
All matching rows will be under `result`.
If `columns` is `NULL` and `columns_count` is `0`, adds the entire row. Otherwise only the columns in `columns`.
`extra_data` is passed to `condition` as-is for your convenience.

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

---

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

---

### Example

    #include <leansql.h>
    #include <stdio.h>
    #include <stdlib.h>

    void printTable(struct LeanSQL_ResultSet table);
    bool condition(wchar_t* column_name, wchar_t* value);

    int main()
    {
        LeanSQL_init("db");

        wchar_t* columns[] = { L"ID", L"Name", L"Email" };
        struct LeanSQL_ActionReport table_creation = LeanSQL_createTable(L"Students", columns, 3);
        if (table_creation.success)
        {
            wchar_t* data1[] = { L"516519878", L"Ivan Rubinson", L"soryy708@gmail.com" };
            wchar_t* data2[] = { L"645951238", L"Josh", L"joshman@gmail.com" };
            wchar_t* data3[] = { L"315312321", L"Drake", L"darken@gmail.com" };
            wchar_t* data4[] = { L"876432453", L"Steve", L"stevenson@gmail.com" };
            LeanSQL_insert(L"Students", data1, 3);
            LeanSQL_insert(L"Students", data2, 3);
            LeanSQL_insert(L"Students", data3, 3);
            LeanSQL_insert(L"Students", data4, 3);

            LeanSQL_delete(L"Students", condition);

            struct LeanSQL_ActionReport select1 = LeanSQL_select(L"Students", NULL, 0, NULL);

            if (select1.success)
            {
                wprintf(L"Select 1:\n");
                printTable(select1.result);
            }
            else
            {
                if (!select1.success)
                {
                    wprintf(L"Select 1 failed: %d\n", select1.error);
                }
            }

            LeanSQL_dropTable(L"Students");
        }
        else
        {
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    void printTable(struct LeanSQL_ResultSet table)
    {
        for (unsigned int i = 0; i < table.rows; ++i)
        {
            for (unsigned int j = 0; j < table.columns; ++j)
            {
                wprintf(L"%s\t", table.data[i][j]);
            }
            wprintf(L"\n");
        }
    }

    bool condition(wchar_t* column_name, wchar_t* value)
    {
        return wcscmp(column_name, L"Name") == 0 && wcscmp(value, L"Josh") == 0;
    }
