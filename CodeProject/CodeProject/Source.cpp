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
#include "WebGenerator.h"

void FindBest()
{
	double currentBest = -std::numeric_limits<double>::max();

	double bestBuy = 0.0;
	double bestSell = 0.0;

	gBuySmoothWeight = 0.7;

	while (gBuySmoothWeight < 1.0)
	{
		gSellSmoothWeight = 0.7;

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
				std::cout << std::fixed << " Profit (" << currentBest << "): "
					<< bestBuy << ", " << bestSell << "\n";
			}

			gMarket.Reset();
			gModel.Reset();

			gSellSmoothWeight += 0.01;
		}
		gBuySmoothWeight += 0.01;
	}
}

void Analyze()
{
	gBuySmoothWeight = 0.987;
	gSellSmoothWeight = 0.97;

	while (gMarket.AdvanceTime())
	{
		gModel.UpdateTrades();
	}

	gMarket.SaveTrades("D:\\ProjectHillside\\web\\annotations.js");

	gModel.m_OriginalCurve.Save("D:\\ProjectHillside\\web\\graphs\\original.csv");
	gModel.m_SmoothedCurve.Save("D:\\ProjectHillside\\web\\graphs\\smoothed.csv");
	gModel.m_DerivativeCurve.Save("D:\\ProjectHillside\\web\\graphs\\derivative.csv");
	gModel.m_SmoothedDerivativeCurve.Save("D:\\ProjectHillside\\web\\graphs\\smoothedDerivative.csv");

 	WebGenerator gen;
 	gen.PushGraph("graphs/original.csv");
	gen.PushGraph("graphs/smoothed.csv", GraphType::Annotated);
	gen.PushGraph("graphs/derivative.csv", GraphType::PositiveNegative);
	gen.PushGraph("graphs/smoothedDerivative.csv", GraphType::PositiveNegative);
 	gen.Generate("D:\\ProjectHillside\\web\\generated.html");

	auto& trades = gMarket.m_TradeHistory;
	auto totalProfit = std::accumulate(trades.begin(), trades.end(), 0.0, [](auto d, auto* t)
	{
		return d + t->m_Profit;
	});
	std::cout << totalProfit << std::endl;
}

int main()
{

	if (gAppMode == AppMode::Analyze) Analyze();
	else FindBest();

	system("pause");

}