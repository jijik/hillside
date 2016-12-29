#pragma once

#include "Model.h"
#include "Market.h"
#include <sstream>

extern Model gModel;

extern Market gMarket;

extern double pi;

inline double SmoothValue(double originalValue, double newValue, double oldValWeight)
{
	return originalValue * oldValWeight + newValue * (1.0 - oldValWeight);
}

extern double gBuySmoothWeight;
extern double gSellSmoothWeight;

extern double gIdealCurveMovingAvgHistoryNeighbors;
extern double gIdealCurveMovingAvgFutureNeighbors;

template <typename Stream>
inline void	AppendSuffix(Stream& ss, const char* suff)
{
	ss << "_" << int(10000 * gMarket.m_From) << "_" << int(10000 * gMarket.m_To) << suff;
}

namespace AppMode 
{
	enum type
	{
		FindBest,
		Analyze
	};
}

extern AppMode::type gAppMode;

extern double gTotalProfit;