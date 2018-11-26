#define _CRT_SECURE_NO_WARNINGS
#include <leansql.h>
#include <leansql/directory.h>
#include <wchar.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct LeanSQL_ActionReport LeanSQL_insert(wchar_t* table, wchar_t** data, unsigned int data_count)
{
	struct LeanSQL_ActionReport action_report;

	if (table == NULL || wcscmp(table, L"") == 0 || data == NULL || data_count == 0)
	{
		action_report.success = false;
		action_report.error = LEANSQL_ERROR_PARAMETER;
		return action_report;
	}

	char* path = LeanSQL_getPath(table);
	if (path == NULL)
	{
		action_report.success = false;
		action_report.error = LEANSQL_ERROR_MEMORY;
		return action_report;
	}

	FILE* file = fopen(path, "r");
	if (file == NULL)
	{
		action_report.success = false;
		action_report.error = LEANSQL_ERROR_NO_TABLE;
		free(path);
		return action_report;
	}

	unsigned int table_columns = 1;
	int ch;
	ch = getc(file);
	while (ch != EOF && ch != '\n')
	{
		if (ch == '\t')
			++table_columns;
		ch = getc(file);
	}
	if (table_columns != data_count)
	{
		action_report.success = false;
		action_report.error = LEANSQL_ERROR_TABLE_MISMATCH;
		free(path);
		fclose(file);
		return action_report;
	}

	file = freopen(path, "a", file);
	if (file == NULL)
	{
		action_report.success = false;
		action_report.error = LEANSQL_ERROR_IO;
		free(path);
		return action_report;
	}

	free(path);

	for (unsigned int i = 0; i < data_count; ++i)
	{
		fwprintf(file, L"%s", data[i]);
		if (i + 1 < data_count)
		{
			fwprintf(file, L"\t");
		}
		else
		{
			fwprintf(file, L"\n");
		}
	}

	fclose(file);
	action_report.result.rows = 1;
	action_report.result.columns = data_count;
	action_report.success = true;
	return action_report;
}
