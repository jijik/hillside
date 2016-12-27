#include <iterator>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <iomanip>
#include <numeric>
#include <string>

#include "CSVReader.h"
#include "Curve.h"
#include "Trade.h"
#include "Market.h"
#include "genv.h"

int main()
{
	double currentBest = -std::numeric_limits<double>::max();

	double bestBuy = 0.0;
	double bestSell = 0.0;

	gBuySmoothWeight = 0.7;

	while(gBuySmoothWeight < 1.0)
	{
		gSellSmoothWeight = 0.7;
//		std::cout << "Trying [b]: " << std::fixed << gBuySmoothWeight;

		while (gSellSmoothWeight < 1.0)
		{
			//
			while (gMarket.AdvanceTime())
			{
				gModel.UpdateTrades();
			}


			auto& trades = gMarket.m_TradeHistory;
			auto totalProfit = std::accumulate(trades.begin(), trades.end(), 0.0, [](auto d, auto* t) 
			{
				return d + t->m_Profit; 
			});

			if (totalProfit > currentBest)
			{
				currentBest = totalProfit;

				bestBuy = gBuySmoothWeight;
				bestSell = gSellSmoothWeight;
				gMarket.SaveTrades("D:\\ProjectHillside\\web\\annotations.js");
			
				std::cout.precision(17);
				std::cout << std::fixed << " Profit (" << currentBest <<"): "
					<< bestBuy << ", " << bestSell << "\n";
			}
			
			gMarket.Reset();
			gModel.Reset();

			gSellSmoothWeight += 0.004;
		}
		gBuySmoothWeight += 0.004;
	}



	system("pause");
}