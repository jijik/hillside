#include "Market.h"
#include <ostream>
#include <fstream>
#include <algorithm>

const double Market::ratio = 0.9975;

//////////////////////////////////////////////////////////////////////////
Market::Market()
{
// 	m_OriginalCurve.Load("D:\\ProjectHillside\\SourceData\\krakenUSD.csv");
// 	m_OriginalCurve.BinarySave("D:\\ProjectHillside\\SourceData\\krakenUSD.bin");

	m_OriginalCurve.BinaryLoad("D:\\ProjectHillside\\SourceData\\krakenUSD.bin");

	m_OriginalCurve.Crop(0.5, 0.81);
	m_OriginalCurve = std::move(m_OriginalCurve.CreateMovingAverage(15));
	m_OriginalCurve.Save("D:\\ProjectHillside\\web\\graph.csv", Market::ratio);

	m_OriginalCurve.CreateDerivate()./*CreateMovingAverage(2).*/Save("D:\\ProjectHillside\\web\\derivate.csv");
}

//////////////////////////////////////////////////////////////////////////
void Market::Reset()
{
	for (auto* t : m_TradeHistory)
	{
		delete t;
	}
	m_TradeHistory.clear();

	m_CurrentDataIndex = 0;
}

//////////////////////////////////////////////////////////////////////////
bool Market::AdvanceTime()
{
	++m_CurrentDataIndex;
	return m_CurrentDataIndex < m_OriginalCurve.m_Data.size() - 1;
}

//////////////////////////////////////////////////////////////////////////
double Market::GetCurrentPrice()
{
	return m_OriginalCurve.m_Data[m_CurrentDataIndex];
}

//////////////////////////////////////////////////////////////////////////
double Market::GetNextPrice()
{
	if (m_OriginalCurve.m_Data.size() > m_CurrentDataIndex + 1)
	{
		return m_OriginalCurve.m_Data[m_CurrentDataIndex + 1];
	}
	return 0.0;
}

//////////////////////////////////////////////////////////////////////////
void Market::Buy(double amount, Trade& trade)
{
	trade.Buy(amount, GetCurrentPrice(), m_CurrentDataIndex);
}

//////////////////////////////////////////////////////////////////////////
void Market::Sell(Trade& t)
{
	t.m_SoldDataIndex = m_CurrentDataIndex;
	auto soldAt = GetCurrentPrice() * Market::ratio;
	t.m_Profit = soldAt - t.m_BoughtAtPrice;

	m_TradeHistory.push_back(&t);
}

//////////////////////////////////////////////////////////////////////////
void Market::SaveTrades(const char* path)
{
	std::ofstream file(path);
	file << "g.ready(function(){g.setAnnotations([\n";
	for (auto* t : m_TradeHistory)
	{
		file << "{";
		file << "series : \"value\",";
		file << "x : " << t->m_BoughtDataIndex << ",";
		file << "shortText : \"" << "B" << "\",";
		file << "text : \"Bought at " << t->m_BoughtDataIndex << "\"";
		file << "},\n";
		file << "{";
		file << "series : \"value\",";
		file << "x : " << t->m_SoldDataIndex << ",";
		file << "shortText : \"" << "S" << "\",";
		file << "text : \"Sold from " << t->m_BoughtDataIndex << " profit: " << t->m_Profit <<  "\"";
		file << ",cssClass : \"" << (t->m_Profit > 0.0 ? "positive" : "negative") << "Profit\"";
		file << "},\n";
	}
	file << "]);});";
}

