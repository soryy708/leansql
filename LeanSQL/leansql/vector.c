#include "vector.h"
#include <stdlib.h>

struct LeanSQL_Vector
{
	void** data;
	unsigned int size;
};

struct LeanSQL_Vector* vector_create()
{
	struct LeanSQL_Vector* vector = malloc(sizeof(struct LeanSQL_Vector));
	if (vector != NULL)
	{
		vector->data = NULL;
		vector->size = 0;
	}
	return vector;
}

void vector_destroy(struct LeanSQL_Vector* vector)
{
	if (vector != NULL)
	{
		if (vector->data != NULL)
		{
			free(vector->data);
			vector->data = NULL;
			vector->size = 0;
		}
		free(vector);
	}
}

unsigned int vector_getSize(struct LeanSQL_Vector* vector)
{
	if (vector != NULL)
	{
		return vector->size;
	}
	return 0;
}

void vector_resize(struct LeanSQL_Vector* vector, unsigned int size)
{
	if (vector != NULL)
	{
		void** temp = vector->data;
		vector->data = malloc(sizeof(void*) * size); // TODO: rewrite with realloc?
		if (vector->data != NULL)
		{
			unsigned int i = 0;

			if (temp != NULL)
			{
				for (i = 0; i < size && i < vector->size; ++i)
				{
					vector->data[i] = temp[i];
				}
				free(temp);
			}

			for (; i < size; ++i)
			{
				vector->data[i] = NULL;
			}

			vector->size = size;
		}
	}
}

void* vector_getAt(struct LeanSQL_Vector* vector, unsigned int index)
{
	if (vector != NULL && index < vector->size)
	{
		return vector->data[index];
	}
	return NULL;
}

void vector_setAt(struct LeanSQL_Vector* vector, unsigned int index, void* data)
{
	if (vector != NULL && index < vector->size)
	{
		vector->data[index] = data;
	}
}

struct LeanSQL_Vector* vector_copy(struct LeanSQL_Vector* source_vector)
{
	struct LeanSQL_Vector* result_vector = NULL;
	if (source_vector != NULL)
	{
		result_vector = vector_create();
		if (result_vector != NULL)
		{
			vector_resize(result_vector, source_vector->size);
			if (result_vector->data != NULL && source_vector->data != NULL)
			{
				for (unsigned int i = 0; i < source_vector->size; ++i)
				{
					result_vector->data[i] = source_vector->data[i];
				}
			}
		}
	}
	return result_vector;
}

void vector_push(struct LeanSQL_Vector* vector, void* data)
{
	if (vector != NULL && data != NULL)
	{
		vector_resize(vector, vector->size + 1);
		vector->data[vector, vector->size - 1] = data;
	}
}
