#define _CRT_SECURE_NO_WARNINGS
#include <leansql.h>
#include <leansql/directory.h>
#include <wchar.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct LeanSQL_ActionReport LeanSQL_dropTable(wchar_t* table)
{
	struct LeanSQL_ActionReport action_report;

	if (table == NULL || wcscmp(table, L"") == 0)
	{
		action_report.success = false;
		action_report.error = LEANSQL_ERROR_PARAMETER;
		return action_report;
	}

	char* path = LeanSQL_getPath(table);
	if (path == NULL)
	{
		action_report.error = LEANSQL_ERROR_MEMORY;
		return action_report;
	}

	int status = remove(path);
	if (status == 0)
	{
		action_report.success = true;
	}
	else
	{
		action_report.success = false;
		action_report.error = LEANSQL_ERROR_NO_TABLE;
	}

	free(path);
	return action_report;
}
