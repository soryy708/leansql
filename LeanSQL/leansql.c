#include <leansql.h>
#include <leansql/directory.h>
#include <stdlib.h>

void LeanSQL_init(char* directory)
{
	LeanSQL_setDirectory(directory);
}

void LeanSQL_destroyResultSet(struct LeanSQL_ResultSet* set)
{
	if (set == NULL)
	{
		return;
	}

	for (unsigned int i = 0; i < set->rows; ++i)
	{
		for (unsigned int j = 0; j < set->columns; ++j)
		{
			free(set->data[i][j]);
		}
		free(set->data[i]);
	}
	free(set->data);
	set->data = NULL;
}
