#pragma once

#include <span>

void populate_hamming_factors(std::span<float> factors, float alpha);
void populate_blackman_harris_factors(std::span<float> factors, float alpha);
void populate_rectangle_factors(std::span<float> factors);
