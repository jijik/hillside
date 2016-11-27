#include <iterator>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <string>

class CSVRow
{
public:
	std::string const& operator[](std::size_t index) const
	{
		return m_data[index];
	}
	std::size_t size() const
	{
		return m_data.size();
	}
	void readNextRow(std::istream& str)
	{
		std::string         line;
		std::getline(str, line);

		std::stringstream   lineStream(line);
		std::string         cell;

		m_data.clear();
		while (std::getline(lineStream, cell, ','))
		{
			m_data.push_back(cell);
		}
	}
private:
	std::vector<std::string>	m_data;
};

std::istream& operator >> (std::istream& str, CSVRow& data)
{
	data.readNextRow(str);
	return str;
}


struct Curve
{
	Curve() = default;
	Curve(Curve&& rhs)
		: m_Data(std::move(rhs.m_Data))
	{
		
	}

	void Load(const char* path)
	{
		std::ifstream file(path);
		CSVRow row;
		while (file >> row)
		{
			m_Data.push_back(std::stod(row[1]));
		}
	}

	void Save(const char* path, bool target = false)
	{
		std::ofstream file(path);
		unsigned i = 0;
		if (target)
			file << i << ", value, target" << "\n";
		else
			file << i << ", value" << "\n";

		for (auto value : m_Data)
		{
			if(target)
				file << ++i << "," << value << "," << value + 4 << "\n";
			else
				file << ++i << "," << value << "\n";
		}
		file.close();
	}

	Curve CreateMovingAverage(unsigned neighborCount)
	{
		int size = m_Data.size();
		Curve c;
		c.m_Data.resize(size);
		for (int i = 0; i < size; ++i)
		{
			int from = i - neighborCount;
			int to = i + neighborCount;
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
		return c;
	}

	void Crop(double begin, double end) //0-1
	{
		auto size = m_Data.size();
		unsigned b = size * begin;
		unsigned e = size * end;

		e = std::min(e, size);

		m_Data = std::vector<double>(&m_Data[b], &m_Data[e]);
	}

	std::vector<double> m_Data;
};

std::vector<std::pair<unsigned, bool>> trades;
void SaveTrades(const char* path)
{
	std::ofstream file(path);
	file << "g.ready(function(){g.setAnnotations([\n";
	for (auto& p : trades)
	{
		file << "{";
		file << "series : \"value\",";
		file << "x : " << p.first << ",";
		file << "shortText : \"" << p.second << "\",";
		file << "text : \"ASD\"";
		file << "},\n";
	}
	file << "]);});";
}

int main()
{
	Curve originalCurve;
	originalCurve.Load("D:\\tmp\\all.csv");
	originalCurve.Crop(0.97, 0.98);

	auto newCurve = originalCurve.CreateMovingAverage(15);

	newCurve.Save("D:\\project_hillside\\web\\graph.csv", true);

	double current = newCurve.m_Data[0];

	enum State { none, bought, sold} state = none;

	Curve sumCurve;

	double sum = 0.0;
	double boughtAtPrice = 0.0;

	double ratio = 0.99;

	auto& data = newCurve.m_Data;
	auto size = newCurve.m_Data.size() - 1;
	for (unsigned i = 1; i < size; ++i)
	{
		double currentValue = data[i];
		bool buy = (data[i - 1] > currentValue && currentValue < data[i + 1]);
		bool sell = (data[i - 1] < currentValue && currentValue > data[i + 1]);

		if (state == bought)
		{
			sell &= boughtAtPrice * ratio < currentValue;	// immediate sell can be bad
			sell |= currentValue / boughtAtPrice < 0.98; // stop loss order
		}

		if (buy)
		{
			boughtAtPrice = data[i];
			state = bought;
			trades.emplace_back(i, true);
		}
		else if (sell && state != none)
		{
			double diff = ratio*data[i] - boughtAtPrice;
			sum += diff;
			sumCurve.m_Data.push_back(sum);
			trades.emplace_back(i, false);
			state = sold;
		}
	}

	std::cout << sum;
	sumCurve.Save("D:\\project_hillside\\web\\sumCurve.csv");
	SaveTrades("D:\\project_hillside\\web\\annotations.js");

	system("pause");
}