#define _CRT_SECURE_NO_WARNINGS
#include <leansql.h>
#include <leansql/directory.h>
#include <wchar.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct LeanSQL_ActionReport LeanSQL_createTable(wchar_t* title, wchar_t** columns, unsigned int columns_count)
{
	struct LeanSQL_ActionReport action_report;
	if (title == NULL || wcscmp(title, L"") == 0 || columns == NULL || columns_count == 0)
	{
		action_report.success = false;
		action_report.error = LEANSQL_ERROR_PARAMETER;
		return action_report;
	}

	char* path = LeanSQL_getPath(title);
	if (path == NULL)
	{
		action_report.success = false;
		action_report.error = LEANSQL_ERROR_MEMORY;
		return action_report;
	}

	FILE* file = fopen(path, "w");
	if (file == NULL)
	{
		action_report.success = false;
		action_report.error = LEANSQL_ERROR_IO;
		free(path);
		return action_report;
	}
	free(path);

	for (unsigned int i = 0; i < columns_count; ++i)
	{
		fwprintf(file, L"%s", columns[i]);
		if (i + 1 < columns_count)
		{
			fwprintf(file, L"\t");
		}
		else
		{
			fwprintf(file, L"\n");
		}
	}
	fclose(file);
	action_report.success = true;
	return action_report;
}
