#pragma once

#include "Combinator.h"

void FindMinimalCurveError()
{
	Curve originalCurve, idealCurve;

	std::ifstream file("D:\\ProjectHillside\\web\\graphs\\idealCurve_4200_4250.csv");
	CSVRow row;
	while (file >> row)
	{
		originalCurve.m_Data.push_back(std::stod(row[1]));
		idealCurve.m_Data.push_back(std::stod(row[2]));
	}

	double movingAverageHistory = 0.0;

	Combinator combinator;
	combinator.AddVariable(movingAverageHistory, 1.1, 30.1, 1.0);

	double currentBest = std::numeric_limits<double>::max();

	combinator.Combine([&]() 
	{
		Curve moved = originalCurve.CreateMovingAverage(static_cast<unsigned>(movingAverageHistory), 1);

		double err = idealCurve.CalculateAverageError(moved);

		if (err < currentBest)
		{
			std::cout << "History: " << movingAverageHistory << ", Error: " << err << "\n";
			currentBest = err;
		}

	}, 20.0);

	system("pause");
}
