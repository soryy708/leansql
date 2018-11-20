#define _CRT_SECURE_NO_WARNINGS
#include <leansql/util.h>
#include <wchar.h>
#include <stdlib.h>
#include <Windows.h> // for SetEndOfFile
#include <io.h> // for _get_osfhandle

struct LeanSQL_Vector* LeanSQL_parseLine(FILE* file)
{
	struct LeanSQL_Vector* vector = vector_create();
	if (vector == NULL)
	{
		return NULL;
	}

	wchar_t line[1024];
	if (fgetws(line, 1024, file) == NULL)
	{
		vector_destroy(vector);
		return NULL;
	}

	wchar_t* token;
	wchar_t* context = NULL;

	token = wcstok(line, L"\t\n", &context);
	if (token != NULL)
	{
		wchar_t* str = malloc(sizeof(wchar_t) * (wcslen(token) + 1));
		if (str == NULL)
		{
			for (unsigned int i = 0; i < vector_getSize(vector); ++i)
			{
				free(vector_getAt(vector, i));
			}
			vector_destroy(vector);
			return NULL;
		}
		wcscpy(str, token);
		vector_push(vector, str);
		while ((token = wcstok(NULL, L"\t\n", &context)) != NULL)
		{
			str = malloc(sizeof(wchar_t) * (wcslen(token) + 1));
			if (str == NULL)
			{
				for (unsigned int i = 0; i < vector_getSize(vector); ++i)
				{
					free(vector_getAt(vector, i));
				}
				vector_destroy(vector);
				return NULL;
			}
			wcscpy(str, token);
			vector_push(vector, str);
		}
	}

	return vector;
}

void LeanSQL_shiftFileContents(FILE* file, long shift)
{
	if (file == NULL || shift == 0)
	{
		return;
	}

	long shift_start_pos = ftell(file);
	
	long remainder_length = 0;
	for (char ch = getc(file); ch != EOF; ch = getc(file))
	{
		++remainder_length;
	}
	
	fseek(file, shift_start_pos, SEEK_SET);
	char* buffer = malloc(remainder_length);
	if (buffer != NULL)
	{
		long i = 0;
		for (char ch = getc(file); ch != EOF && i < remainder_length; ch = getc(file), ++i)
		{
			buffer[i] = ch;
		}

		fseek(file, shift_start_pos + shift, SEEK_SET);

		for (long i = 0; i < remainder_length; ++i)
		{
			fprintf(file, "%c", buffer[i]);
		}

		free(buffer);
		
		if (shift < 0)
		{
			// We need to truncate the file.
			// This is OS-dependant.
			// We need to convert a standard FILE* to a Win32 HANDLE.
			SetEndOfFile(_get_osfhandle(_fileno(file)));
		}
	}
	else
	{
		// TODO
	}
}
