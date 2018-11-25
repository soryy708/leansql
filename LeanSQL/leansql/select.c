#define _CRT_SECURE_NO_WARNINGS
#include <leansql.h>
#include <leansql/vector.h>
#include <leansql/directory.h>
#include <leansql/util.h>
#include <wchar.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct LeanSQL_ActionReport LeanSQL_select(wchar_t* table, wchar_t** columns, unsigned int columns_count, bool(*condition)(wchar_t*, wchar_t*, void*), void* extra_data)
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

	free(path);

	struct LeanSQL_Vector* header = LeanSQL_parseLine(file);
	if (header == NULL)
	{
		action_report.success = false;
		action_report.error = LEANSQL_ERROR_IO;
		fclose(file);
		return action_report;
	}

	struct LeanSQL_Vector* results_matrix = vector_create();
	if (results_matrix == NULL)
	{
		action_report.success = false;
		action_report.error = LEANSQL_ERROR_IO;
		fclose(file);
		vector_destroy(header);
		return action_report;
	}

	struct LeanSQL_Vector* line;
	bool should_read = true;
	while (should_read)
	{
		line = LeanSQL_parseLine(file);
		if (line != NULL)
		{
			unsigned int column_count = vector_getSize(header);
			if (column_count != vector_getSize(line))
			{
				action_report.success = false;
				action_report.error = LEANSQL_ERROR_TABLE_MALFORMED;
				fclose(file);
				vector_destroy(header);
				for (unsigned int i = 0; i < vector_getSize(results_matrix); ++i)
				{
					vector_destroy(vector_getAt(results_matrix, i));
				}
				vector_destroy(results_matrix);
				vector_destroy(line);
				return action_report;
			}

			bool should_add = false;
			if (condition != NULL)
			{
				for (unsigned int i = 0; i < column_count && !should_add; ++i)
				{
					if (condition(vector_getAt(header, i), vector_getAt(line, i), extra_data))
					{
						should_add = true;
					}
				}
			}
			else
			{
				should_add = true;
			}

			if (should_add)
			{
				struct LeanSQL_Vector* vector = vector_create();
				if (vector == NULL)
				{
					action_report.success = false;
					action_report.error = LEANSQL_ERROR_MEMORY;
					fclose(file);
					vector_destroy(header);
					for (unsigned int i = 0; i < vector_getSize(results_matrix); ++i)
					{
						vector_destroy(vector_getAt(results_matrix, i));
					}
					vector_destroy(results_matrix);
					vector_destroy(line);
					return action_report;
				}

				for (unsigned int i = 0; i < column_count; ++i)
				{
					bool should_add_column = false;
					if (columns != NULL && columns_count > 0)
					{
						for (unsigned int j = 0; j < columns_count; ++j)
						{
							if (wcscmp(vector_getAt(header, i), columns[j]) == 0)
							{
								should_add_column = true;
							}
						}
					}
					else
					{
						should_add_column = true;
					}

					if (should_add_column)
					{
						vector_push(vector, vector_getAt(line, i));
					}
				}

				vector_push(results_matrix, vector);
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

	action_report.result.rows = vector_getSize(results_matrix);
	action_report.result.columns = vector_getSize(vector_getAt(results_matrix, 0));

	wchar_t*** matrix = malloc(sizeof(wchar_t**) * action_report.result.rows);
	if (matrix == NULL)
	{
		action_report.success = false;
		action_report.error = LEANSQL_ERROR_MEMORY;
		for (unsigned int i = 0; i < vector_getSize(results_matrix); ++i)
		{
			vector_destroy(vector_getAt(results_matrix, i));
		}
		vector_destroy(results_matrix);
		return action_report;
	}

	for (unsigned int i = 0; i < action_report.result.rows; ++i)
	{
		wchar_t** row = malloc(sizeof(wchar_t*) * action_report.result.columns);
		if (row == NULL)
		{
			action_report.success = false;
			action_report.error = LEANSQL_ERROR_MEMORY;
			for (unsigned int j = 0; j < i; ++j)
			{
				for (unsigned int k = 0; k < action_report.result.columns; ++k)
				{
					free(matrix[j][k]);
				}
				free(matrix[j]);
			}
			free(matrix);
			for (unsigned int i = 0; i < vector_getSize(results_matrix); ++i)
			{
				vector_destroy(vector_getAt(results_matrix, i));
			}
			vector_destroy(results_matrix);
			return action_report;
		}

		for (unsigned int j = 0; j < action_report.result.columns; ++j)
		{
			row[j] = vector_getAt(vector_getAt(results_matrix, i), j);
		}

		matrix[i] = row;
	}

	for (unsigned int i = 0; i < vector_getSize(results_matrix); ++i)
	{
		vector_destroy(vector_getAt(results_matrix, i));
	}
	vector_destroy(results_matrix);

	action_report.success = true;
	action_report.result.data = matrix;
	return action_report;
}
