#include "pch.h"
#include "FractalBrownianMotion.h"
#define FNL_IMPL
#include "FastNoiseLite.h"
#include <stdlib.h>

FractalBrownianMotion::FractalBrownianMotion(int _octaves, float _lacunarity, float _frecuency)
{
	octaves = _octaves;
	lacunarity = _lacunarity;
	frecuency = _frecuency;
	seed = rand() % 10000 + 1;     // randomrange 1 to 10000;
}

float FractalBrownianMotion::getNoise(int x, int y, int z)
{
	return 0.0f;
}
