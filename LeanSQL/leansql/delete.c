#define _CRT_SECURE_NO_WARNINGS
#include <leansql.h>
#include <leansql/directory.h>
#include <leansql/util.h>
#include <wchar.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct LeanSQL_ActionReport LeanSQL_delete(wchar_t* table, bool(*condition)(wchar_t*, wchar_t*, void*), void* extra_data)
{
	struct LeanSQL_ActionReport action_report;

	if (table == NULL || wcscmp(table, L"") == 0 || condition == NULL)
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

	FILE* file = fopen(path, "r+");
	if (file == NULL)
	{
		action_report.success = false;
		action_report.error = LEANSQL_ERROR_NO_TABLE;
		free(path);
		return action_report;
	}

	free(path);

	struct LeanSQL_Vector* header = LeanSQL_parseLine(file);
	if (header == NULL)
	{
		action_report.success = false;
		action_report.error = LEANSQL_ERROR_IO;
		fclose(file);
		return action_report;
	}

	action_report.result.columns = vector_getSize(header);
	action_report.result.rows = 0;
	long prev_line_pos = 0;
	long cur_line_pos = 0;
	struct LeanSQL_Vector* line;
	bool should_read = true;
	while (should_read)
	{
		prev_line_pos = ftell(file);
		line = LeanSQL_parseLine(file);
		cur_line_pos = ftell(file);
		if (line != NULL)
		{
			unsigned int column_count = vector_getSize(header);
			if (column_count != vector_getSize(line))
			{
				action_report.success = false;
				action_report.error = LEANSQL_ERROR_TABLE_MALFORMED;
				fclose(file);
				vector_destroy(header);
				vector_destroy(line);
				return action_report;
			}

			bool should_delete = false;
			for (unsigned int i = 0; i < column_count && !should_delete; ++i)
			{
				if (condition(vector_getAt(header, i), vector_getAt(line, i), extra_data))
				{
					should_delete = true;
				}
			}

			if (should_delete)
			{
				long line_length = 0;
				for (unsigned int i = 0; i < column_count; ++i)
				{
					line_length += wcslen(vector_getAt(line, i));
					++line_length; // \t or \n
				}
				LeanSQL_shiftFileContents(file, line_length * -1);
				fflush(file);

				++action_report.result.rows;
			}

			vector_destroy(line);
		}
		else
		{
			should_read = false;
		}
	}

	fclose(file);
	vector_destroy(header);

	action_report.success = true;
	return action_report;
}
