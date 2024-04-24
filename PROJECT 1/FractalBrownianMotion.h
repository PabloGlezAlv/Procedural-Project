#pragma once

class FractalBrownianMotion
{
public:
	FractalBrownianMotion(){}

	FractalBrownianMotion(int _octaves, float _lacunarity, float _frecuency);

	float getNoise(int x, int y, int z);
private:
	int seed = 0;
	int octaves = 3;
	float lacunarity = 2;
	float frecuency = 0.005f;
};

