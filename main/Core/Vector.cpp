#include "stdafx.h"
#include "Core/Vector.h"

template<>
void initArray(bool *data, size_t from, size_t to) {}

template<>
void initArray(short *data, size_t from, size_t to) {}

template<>
void initArray(int *data, size_t from, size_t to) {}

template<>
void initArray(size_t *data, size_t from, size_t to) {}

template<>
void initArray(double *data, size_t from, size_t to) {}
