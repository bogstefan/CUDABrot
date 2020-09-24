#pragma once
#include <array>
#include "Utility.h"
#include "Buffer.h"

void Calculate(unsigned int* pixels, Bounds bounds, ScreenDim dim, int iterations, const mb::Buffer& buffer);
void SetLookup(const std::array<Color, 256>& palette);
void InitCuda(const mb::Buffer& buffer);
