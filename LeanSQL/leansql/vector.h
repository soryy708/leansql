#pragma once

#include <stdbool.h>

struct LeanSQL_Vector;

struct LeanSQL_Vector* vector_create();
void vector_destroy(struct LeanSQL_Vector*);
unsigned int vector_getSize(struct LeanSQL_Vector*);
void vector_resize(struct LeanSQL_Vector*, unsigned int size);
void* vector_getAt(struct LeanSQL_Vector*, unsigned int index);
void vector_setAt(struct LeanSQL_Vector*, unsigned int index, void* data);
void vector_push(struct LeanSQL_Vector*, void* data);
