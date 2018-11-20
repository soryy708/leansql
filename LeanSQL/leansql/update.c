#define _CRT_SECURE_NO_WARNINGS
#include <leansql.h>
#include <leansql/directory.h>
#include <leansql/vector.h>
#include <leansql/util.h>
#include <wchar.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct LeanSQL_ActionReport LeanSQL_update(wchar_t* table, wchar_t** data, wchar_t** columns, unsigned int columns_count, bool(*condition)(wchar_t*, wchar_t*))
{
	struct LeanSQL_ActionReport action_report;

	if (table == NULL || wcscmp(table, L"") == 0 || data == NULL || columns_count == 0)
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

	if (vector_getSize(header) < columns_count || (columns == NULL && vector_getSize(header) != columns_count))
	{
		action_report.success = false;
		action_report.error = LEANSQL_ERROR_TABLE_MISMATCH;
		fclose(file);
		vector_destroy(header);
		return action_report;
	}
	
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

			bool should_update = false;
			if (condition != NULL)
			{
				for (unsigned int i = 0; i < column_count && !should_update; ++i)
				{
					if (condition(vector_getAt(header, i), vector_getAt(line, i)))
					{
						should_update = true;
					}
				}
			}
			else
			{
				should_update = true;
			}

			if (should_update)
			{
				for (unsigned int i = 0; i < columns_count; ++i)
				{
					if (columns != NULL)
					{
						for (unsigned int j = 0; j < column_count; ++j)
						{
							if (wcscmp(columns[i], vector_getAt(header, j)) == 0)
							{
								free(vector_getAt(line, j));
								vector_setAt(line, j, data[i]);
							}
						}
					}
					else
					{
						free(vector_getAt(line, i));
						vector_setAt(line, i, data[i]);
					}
				}

				wchar_t buffer[1024];
				buffer[0] = L'\0';
				for (unsigned int i = 0; i < column_count; ++i)
				{
					wcscat(buffer, vector_getAt(line, i));
					if (i + 1 < column_count)
					{
						wcscat(buffer, L"\t");
					}
					else
					{
						wcscat(buffer, L"\n");
					}
				}

				long new_line_length = wcslen(buffer);
				long old_line_length = cur_line_pos - prev_line_pos;
				long shift = new_line_length - old_line_length + 1;

				LeanSQL_shiftFileContents(file, shift);

				fseek(file, prev_line_pos, SEEK_SET);
				fwprintf(file, L"%s", buffer);
				fflush(file);
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
