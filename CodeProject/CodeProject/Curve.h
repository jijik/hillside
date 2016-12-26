#pragma once

#include <vector>

struct Curve
{
	Curve() = default;
	Curve(Curve&& rhs);
	Curve& operator=(const Curve&) = default;

	void	Load(const char* path);
	void	Save(const char* path, double targetRatio = 1.0);

	void	BinaryLoad(const char* path);
	void	BinarySave(const char* path);

	Curve CreateMovingAverage(unsigned neighborCount);
	void	Crop(double begin, double end); //0-1

	std::vector<double> m_Data;
};