#pragma once

#include <leansql/vector.h>
#include <stdio.h>

struct LeanSQL_Vector* LeanSQL_parseLine(FILE* file);
void LeanSQL_shiftFileContents(FILE* file, long shift);
