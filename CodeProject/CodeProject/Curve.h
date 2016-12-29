#pragma once

#include <vector>

struct Curve
{
	Curve() = default;
	Curve(Curve&& rhs);
	Curve& operator=(Curve&&);

	void	Clear();

	void	Load(const char* path);
	void	Save(const char* path, double targetRatio = 1.0);

	void	BinaryLoad(const char* path);
	void	BinarySave(const char* path);

	void	CropInPlace(double begin, double end); //0-1
	Curve	Crop(double begin, double end); //0-1

	Curve	CreateMovingAverage(unsigned neighborCount);
	Curve	CreateMovingAverage(unsigned historyCount, unsigned futureCount);
	Curve	CreateDerivate();
	Curve	CreateSubcurveFromIndex(unsigned index);

	static void SaveCSV(Curve& c1, Curve& c2, const char* path);

	std::vector<double> m_Data;
};