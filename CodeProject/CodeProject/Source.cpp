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
#include "Combinator.h"

void SaveGenv(double profit, const char* path)
{
	std::ofstream file(path);
	AppendSuffix(file, "<br/>\n");
	file << "gIdealCurveMovingAvgHistoryNeighbors " << gIdealCurveMovingAvgHistoryNeighbors << "<br/>\n";
	file << "gIdealCurveMovingAvgFutureNeighbors " << gIdealCurveMovingAvgFutureNeighbors << "<br/>\n";
	file << "gBuySmoothWeight " << gBuySmoothWeight << "<br/>\n";
	file << "gSellSmoothWeight " << gSellSmoothWeight << "<br/>\n";
	file << "profit " << profit << "<br/>\n";
	file.close();
}

double currentBest;

void FindBest(double from, double to)
{
	gMarket.Init(from, to);

	currentBest = -std::numeric_limits<double>::max();

	Combinator combinator;
	combinator.AddVariable(gIdealCurveMovingAvgHistoryNeighbors, 1.1, 3.0, 1.0);
	combinator.AddVariable(gIdealCurveMovingAvgFutureNeighbors, 1.1, 30.0, 1.0);
	combinator.AddVariable(gBuySmoothWeight, 0.8, 0.99, 0.05);
	combinator.AddVariable(gSellSmoothWeight, 0.8, 0.99, 0.05);
	double timeToSearch = 60.0;

	combinator.Combine([&]() 
	{
		gModel.Init();
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

			gMarket.SaveTrades("D:\\ProjectHillside\\web\\annotations.js");

			std::stringstream annotationPath, compareIdealPath;
			annotationPath << "D:\\ProjectHillside\\web\\graphs\\";
			compareIdealPath << annotationPath.str();
			annotationPath << "annotations";
			AppendSuffix(annotationPath, ".js");
			gMarket.SaveTrades(annotationPath.str().c_str());

			compareIdealPath << "idealCurve";
			AppendSuffix(compareIdealPath, ".csv");

			Curve::SaveCSV(gMarket.m_OriginalCurve, gModel.m_IdealCurve, compareIdealPath.str().c_str());
			
			std::stringstream best;
			best << "D:\\ProjectHillside\\web\\graphs\\best";
			AppendSuffix(best, ".txt");
			SaveGenv(totalProfit, best.str().c_str());

			std::cout.precision(17);
			std::cout << std::fixed << " Profit (" << currentBest << "): "
				<< gBuySmoothWeight << ", " << gSellSmoothWeight << " - "
				<< static_cast<unsigned>(gIdealCurveMovingAvgHistoryNeighbors) << " "
				<< static_cast<unsigned>(gIdealCurveMovingAvgFutureNeighbors) << "\n";
		}

		gMarket.Reset();
		gModel.Reset();
	}, timeToSearch);
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

// 	gModel.m_OriginalCurve.Save("D:\\ProjectHillside\\web\\graphs\\original.csv");
// 	gModel.m_SmoothedCurve.Save("D:\\ProjectHillside\\web\\graphs\\smoothed.csv");
// 	gModel.m_DerivativeCurve.Save("D:\\ProjectHillside\\web\\graphs\\derivative.csv");
// 	gModel.m_SmoothedDerivativeCurve.Save("D:\\ProjectHillside\\web\\graphs\\smoothedDerivative.csv");

//  	WebGenerator gen;
//  	gen.PushGraph("graphs/original.csv");
// 	gen.PushGraph("graphs/smoothed.csv", GraphType::Annotated);
// 	gen.PushGraph("graphs/derivative.csv", GraphType::PositiveNegative);
// 	gen.PushGraph("graphs/smoothedDerivative.csv", GraphType::PositiveNegative);
//  	gen.Generate("D:\\ProjectHillside\\web\\generated.html");

	auto& trades = gMarket.m_TradeHistory;
	auto totalProfit = std::accumulate(trades.begin(), trades.end(), 0.0, [](auto d, auto* t)
	{
		return d + t->m_Profit;
	});
	std::cout << totalProfit << std::endl;
}

int main()
{
 	//if (gAppMode == AppMode::Analyze) Analyze();
 	//else FindBest();

// 	Curve testCurve;
// 	testCurve.m_Data = { 1,3,5,4,2,6,5,8,9,7,8,5,6,4,3,2,1 };
// 	Curve c2 = testCurve.CreateMovingAverage(2,3);

	gTotalProfit = 0.0;

	WebGenerator gen;
	
	double from = 0.82;
	while (from < 0.83)
	{
		double to = from + 0.005;
		FindBest(from, to);
		from += 0.005;

		gTotalProfit += currentBest;

		gen.PushCurrentGraph();
	}

	gen.Generate("D:\\ProjectHillside\\web\\generated.html");

	system("pause");

}