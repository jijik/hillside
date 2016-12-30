#include "Curve.h"
#include "CSVReader.h"

#include <iterator>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <string>
#include <cassert>

//////////////////////////////////////////////////////////////////////////
Curve::Curve(Curve&& rhs)
	: m_Data(std::move(rhs.m_Data))
{

}

//////////////////////////////////////////////////////////////////////////
Curve& Curve::operator=(Curve&& rhs)
{
	m_Data = std::move(rhs.m_Data);
	return *this;
}

//////////////////////////////////////////////////////////////////////////
void Curve::Clear()
{
	m_Data.clear();
}

//////////////////////////////////////////////////////////////////////////
void Curve::Load(const char* path)
{
	std::ifstream file(path);
	CSVRow row;
	while (file >> row)
	{
		m_Data.push_back(std::stod(row[1]));
	}
}

//////////////////////////////////////////////////////////////////////////
void Curve::Save(const char* path, double targetRatio /*= 1.0f*/)
{
	std::ofstream file(path);
	unsigned i = 0;
	if (targetRatio != 1.0)
		file << i << ", c1, c2" << "\n";
	else
		file << i << ", c1" << "\n";

	for (auto value : m_Data)
	{
		if (targetRatio != 1.0)
			file << i++ << "," << value << "," << value * (1.0 + (1.0 - targetRatio)) << "\n";
		else
			file << i++ << "," << value << "\n";
	}
	file.close();
}

//////////////////////////////////////////////////////////////////////////
void Curve::BinaryLoad(const char* path)
{
	std::ifstream file(path, std::ios::binary);

	unsigned size;
	file.read((char*)&size, sizeof(unsigned));

	m_Data.clear();
	m_Data.reserve(size);

 	for (unsigned i = 0; i < size; ++i)
 	{
 		double val;
 		file.read((char*)&val, sizeof(double));
 		m_Data.push_back(val);
 	}
}

//////////////////////////////////////////////////////////////////////////
void Curve::BinarySave(const char* path)
{
	std::ofstream file(path, std::ios::binary);
	unsigned size = m_Data.size();

	file.write((char*)&size, sizeof(unsigned));
 	for (double val : m_Data)
 	{
 		file.write((char*)&val, sizeof(double));
 	}

	file.close();
}

//////////////////////////////////////////////////////////////////////////
Curve Curve::CreateMovingAverage(unsigned neighborCount)
{
	return CreateMovingAverage(neighborCount, neighborCount);
}

//////////////////////////////////////////////////////////////////////////
Curve Curve::CreateMovingAverage(unsigned historyCount, unsigned futureCount)
{
	int size = m_Data.size();
	Curve c;
	c.m_Data.resize(size);

	auto range = futureCount + historyCount;

	double lastSum = 0.0;

	for (int i = 0; i < size; ++i)
	{
		int from = i - historyCount;
		int to = i + futureCount;

		if (from < 0 || to > size)	// handle ends
		{
			if (from < 0) from = 0; 
			if (to > size) to = size;

			auto elements = to - from;
			auto localSum = 0.0;
			for (int mi = from; mi < to; ++mi)
			{
				localSum += m_Data[mi];
			}
			c.m_Data[i] = localSum / static_cast<double>(elements);
		}
		else if (from == 0)	// calculate first regular average
		{
			auto elements = to - from;
			auto localSum = 0.0;
			for (int mi = from; mi < to; ++mi)
			{
				localSum += m_Data[mi];
			}
			lastSum = localSum;
			c.m_Data[i] = localSum / static_cast<double>(elements);
		}
		else // calculate general case, reuse
		{
			lastSum = lastSum - m_Data[from - 1] + m_Data[to - 1];
			c.m_Data[i] = lastSum / static_cast<double>(range);
		}
	}
	return c;
}

//////////////////////////////////////////////////////////////////////////
Curve Curve::CreateDerivate()
{
	int size = m_Data.size();
	Curve ret;
	ret.m_Data.resize(size);
	for (int i = 0; i < size - 1; ++i)
	{
		auto x1 = m_Data[i];
		auto x2 = m_Data[i+1];
		auto diff = (x2 - x1);
		ret.m_Data[i] = diff;
		continue;
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////////
Curve Curve::CreateSubcurveFromIndex(unsigned index)
{
	Curve ret;
	ret.m_Data.insert(ret.m_Data.begin(), m_Data.begin() + index , m_Data.end());
	return ret;
}

//////////////////////////////////////////////////////////////////////////
double Curve::CalculateAverageError(Curve& from)
{
	auto size = m_Data.size();
	assert(size == from.m_Data.size());

	double sum = 0.0;

	for (unsigned i = 0; i < size; ++i)
	{
		sum += abs(m_Data[i] - from.m_Data[i]);
	}

	return sum / double(size);
}

//////////////////////////////////////////////////////////////////////////
void Curve::SaveCSV(Curve& c1, Curve& c2, const char* path)
{
	assert(c1.m_Data.size() == c2.m_Data.size());

	std::ofstream file(path);
	unsigned row = 0;
	file << row << ", c1, c2" << "\n";

	for (unsigned i = 0; i < c1.m_Data.size(); ++i)
	{
		file << row++ << "," << c1.m_Data[i] << "," << c2.m_Data[i] << "\n";
	}

	file.close();
}
//////////////////////////////////////////////////////////////////////////
void Curve::CropInPlace(double begin, double end)
{
	auto size = m_Data.size();
	auto b = static_cast<unsigned>(size  * begin);
	auto e = static_cast<unsigned>(size  * end);

	e = std::min(e, size);

	m_Data = std::vector<double>(&m_Data[b], &m_Data[e]);
}


//////////////////////////////////////////////////////////////////////////
Curve Curve::Crop(double begin, double end) //0-1
{
	auto size = m_Data.size();
	auto b = static_cast<unsigned>(size  * begin);
	auto e = static_cast<unsigned>(size  * end);

	e = std::min(e, size);

	Curve ret;
	ret.m_Data = std::vector<double>(&m_Data[b], &m_Data[e]);
	return ret;
}

//////////////////////////////////////////////////////////////////////////
