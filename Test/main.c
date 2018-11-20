#include <leansql.h>
#include <stdio.h>
#include <stdlib.h>

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

int main()
{
	LeanSQL_init("db");

	wchar_t* columns[] = { L"ID", L"Name", L"Email" };
	struct LeanSQL_ActionReport table_creation = LeanSQL_createTable(L"Students", columns, 3);
	if (table_creation.success)
	{
		wchar_t* data1[] = { L"319600276", L"Ivan Rubinson", L"soryy708@gmail.com" };
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
