#define _CRT_SECURE_NO_WARNINGS
#include <leansql/directory.h>
#include <wchar.h>
#include <string.h>
#include <stdio.h>
#include <Windows.h> // for CreateDirectory

#define TABLE_FILENAME_SIZE 64

char* db_directory = NULL;

void LeanSQL_setDirectory(char* directory)
{
	db_directory = directory;
	CreateDirectory(directory, NULL);
}

char* LeanSQL_getPath(wchar_t* table_title)
{
	char table_title_mbs[TABLE_FILENAME_SIZE];
	mbstate_t state;
	wcsrtombs(table_title_mbs, &table_title, TABLE_FILENAME_SIZE, &state);

	unsigned int path_len = strlen(db_directory) + strlen("/") + strlen(table_title_mbs) + strlen(".txt") + 1;
	char* path = malloc(path_len);
	if (path != NULL)
	{
		sprintf(path, "%s/%s.txt", db_directory, table_title_mbs);
	}
	return path;
}
