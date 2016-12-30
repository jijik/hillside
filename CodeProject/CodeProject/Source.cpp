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
#include "MinimalCurveErrorFinder.h"

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
double timeToSearchOneCurve = 10.0;

void FindBest(double from, double to)
{
	gMarket.Init(from, to);

	currentBest = -std::numeric_limits<double>::max();

	Combinator combinator;
	combinator.AddVariable(gIdealCurveMovingAvgHistoryNeighbors, 1.1, 20.0, 1.0);
	combinator.AddVariable(gIdealCurveMovingAvgFutureNeighbors, 2.1, 1.11, 1.0);
	combinator.AddVariable(gBuySmoothWeight, 0.07, 0.99, 0.001);
	combinator.AddVariable(gSellSmoothWeight, 0.07, 0.99, 0.001);

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
	}, timeToSearchOneCurve);
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
// 	FindMinimalCurveError();
// 	return 0;

 	//if (gAppMode == AppMode::Analyze) Analyze();
 	//else FindBest();

// 	Curve testCurve;
// 	testCurve.m_Data = { 1,3,5,4,2,6,5,8,9,7,8,5,6,4,3,2,1 };
// 	Curve c2 = testCurve.CreateMovingAverage(2,3);

	gTotalProfit = 0.0;

	WebGenerator gen;
	
	double advanceStep = 0.005;
	double range = 0.005;
	double begin = 0.42;
	double end = 0.43;

	double totalSearchTimeSec = 1.0 * 60.0;
	timeToSearchOneCurve = totalSearchTimeSec / ((end - begin) / advanceStep);

	double from = begin;
	while (from < end)
	{
		double to = from + range;
		FindBest(from, to);
		from += advanceStep;

		gTotalProfit += currentBest;

		gen.PushCurrentGraph();

		std::cout << unsigned((from - begin) / (end - begin) * 100) << "%\n";
	}

	gen.Generate("D:\\ProjectHillside\\web\\generated.html");

	system("pause");

}