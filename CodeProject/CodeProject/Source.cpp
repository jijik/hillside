#include <iterator>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <string>

#include "CSVReader.h"
#include "Curve.h"
#include "Trade.h"
#include "Market.h"
#include "genv.h"

int main()
{
// 	Curve originalCurve;
// 	originalCurve.Load("D:\\ProjectHillside\\SourceData\\krakenUSD.csv");
// 	originalCurve.Crop(0.95, 0.98);
// 
// 	auto newCurve = originalCurve.CreateMovingAverage(15);
// 
// 	newCurve.Save("D:\\ProjectHillside\\web\\graph.csv", Market::ratio);
// 
// 	double current = newCurve.m_Data[0];
// 
// 	enum State { none, bought, sold } state = none;
// 
// 	Curve sumCurve;
// 
// 	double sum = 0.0;
// 	double boughtAtPrice = 0.0;
// 
// 	auto& data = newCurve.m_Data;
// 	auto size = newCurve.m_Data.size() - 1;
// 	for (unsigned i = 1; i < size; ++i)
// 	{
// 		double currentValue = data[i];
// 		bool buy = (data[i - 1] > currentValue && currentValue < data[i + 1]);
// 		bool sell = (data[i - 1] < currentValue && currentValue > data[i + 1]);
// 
// 		if (state == bought)
// 		{
// 			sell &= boughtAtPrice * Market::ratio < currentValue;	// immediate sell can be bad
// 			sell |= currentValue / boughtAtPrice < 0.98; // stop loss order
// 		}
// 
// 		if (buy)
// 		{
// 			boughtAtPrice = data[i];
// 			state = bought;
// 			trades.emplace_back(i, true);
// 		}
// 		else if (sell && state != none)
// 		{
// 			double diff = Market::ratio*data[i] - boughtAtPrice;
// 			sum += diff;
// 			sumCurve.m_Data.push_back(sum);
// 			trades.emplace_back(i, false);
// 			state = sold;
// 		}
// 	}
// 
// 	std::cout << sum;
// 	sumCurve.Save("D:\\ProjectHillside\\web\\sumCurve.csv");
// 	SaveTrades("D:\\ProjectHillside\\web\\annotations.js");

	//////////////////////////////////////////////////////////////////////////
	using genv::market;
	
	Trade* trade = nullptr;

	auto prevValue = genv::market.GetCurrentPrice();

	while (market.AdvanceTime())
	{
		auto currentValue = genv::market.GetCurrentPrice();
		auto nextValue = genv::market.GetNextPrice();

		if (!trade && prevValue > currentValue && nextValue > currentValue)
		{
			trade = market.Buy(1.0);
		}
		else if (trade && prevValue < currentValue && nextValue < currentValue)
		{
			market.Sell(trade);
			trade = nullptr;
		}

		prevValue = currentValue;
	}

	market.SaveTrades("D:\\ProjectHillside\\web\\annotations.js");


	//////////////////////////////////////////////////////////////////////////

	system("pause");
}