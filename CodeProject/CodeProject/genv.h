#pragma once

#include "Model.h"
#include "Market.h"

extern Model gModel;

extern Market gMarket;

extern double pi;

inline double SmoothValue(double originalValue, double newValue, double oldValWeight)
{
	return originalValue * oldValWeight + newValue * (1.0 - oldValWeight);
}

extern double gBuySmoothWeight;
extern double gSellSmoothWeight;

namespace AppMode 
{
	enum type
	{
		FindBest,
		Analyze
	};
}

extern AppMode::type gAppMode;