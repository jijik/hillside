#include "Market.h"
#include "genv.h"
#include <ostream>
#include <fstream>
#include <sstream>
#include <algorithm>

const double Market::ratio = 0.9975;

//////////////////////////////////////////////////////////////////////////
Market::Market()
{
// 	m_OriginalCurve.Load("D:\\ProjectHillside\\SourceData\\krakenUSD.csv");
// 	m_OriginalCurve.BinarySave("D:\\ProjectHillside\\SourceData\\krakenUSD.bin");

	m_OrigianlFullCurve.BinaryLoad("D:\\ProjectHillside\\SourceData\\krakenUSD.bin");
}

//////////////////////////////////////////////////////////////////////////
void Market::Init(double from, double to)
{
	m_From = from;
	m_To = to;

	m_OriginalCurve = m_OrigianlFullCurve.Crop(from, to);

	std::stringstream path;
	path << "D:\\ProjectHillside\\web\\graphs\\";
	path << "graph_" << int(100*from) << "_" << int(100*to) << ".csv";

	m_OriginalCurve.Save(path.str().c_str());
	m_OriginalCurve.Save("D:\\ProjectHillside\\web\\graph.csv");
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

	file << "var gAnnotation";
	AppendSuffix(file, "");
	file << " = function(g){g.setAnnotations([\n";
	for (auto* t : m_TradeHistory)
	{
		file << "{";
		file << "series : \"c1\",";
		file << "x : " << t->m_BoughtDataIndex << ",";
		file << "shortText : \"" << "B" << "\",";
		file << "text : \"Bought at " << t->m_BoughtDataIndex << "\"";
		file << "},\n";
		file << "{";
		file << "series : \"c1\",";
		file << "x : " << t->m_SoldDataIndex << ",";
		file << "shortText : \"" << "S" << "\",";
		file << "text : \"Sold from " << t->m_BoughtDataIndex << " profit: " << t->m_Profit <<  "\"";
		file << ",cssClass : \"" << (t->m_Profit > 0.0 ? "positive" : "negative") << "Profit\"";
		file << "},\n";
	}
	file << "]);};";
}

