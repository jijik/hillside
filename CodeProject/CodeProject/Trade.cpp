#include "Trade.h"
#include "Market.h"
#include "genv.h"
#include "WebGenerator.h"

//////////////////////////////////////////////////////////////////////////
void Trade::Buy(double amount, double boughtAtPrice, unsigned dataIndex)
{
	m_BoughtAtPrice = boughtAtPrice;
	m_Amount = amount;
	m_BoughtDataIndex = dataIndex;
	m_Zip = boughtAtPrice * (1.0 + (1.0 - Market::ratio));
	m_Bought = true;
}

//////////////////////////////////////////////////////////////////////////
bool Trade::Update(double current)
{
	bool ret = true;

	if (!m_Bought)
	{
		PreBuyUpdate(current);
	}
	else if (m_Bought)
	{
		ret = PreSellUpdate(current);
	}

	m_PrevPrice = current;

	return ret;
}

//////////////////////////////////////////////////////////////////////////
void Trade::PreBuyUpdate(double current)
{
	double smoothedDerivate = 0.0;
	auto derivativeCurve = gModel.m_SmoothedDerivativeCurve.CreateSubcurveFromIndex(m_CreatedDataIndex);
	auto& derivative = derivativeCurve.m_Data;
	for (unsigned i = 0; i < derivative.size(); ++i)
	{
		smoothedDerivate = SmoothValue(smoothedDerivate, derivative[i], gBuySmoothWeight);
	}

	if (derivative.size() > 5 && smoothedDerivate > 0)
	{
		gMarket.Buy(1.0, *this);

// 		gModel.m_OriginalCurve.Save("D:\\ProjectHillside\\web\\graphs\\Aoriginal.csv");
// 		gModel.m_SmoothedCurve.Save("D:\\ProjectHillside\\web\\graphs\\Asmoothed.csv");
// 		gModel.m_DerivativeCurve.Save("D:\\ProjectHillside\\web\\graphs\\Aderivative.csv");
// 		gModel.m_SmoothedDerivativeCurve.Save("D:\\ProjectHillside\\web\\graphs\\AsmoothedDerivative.csv");
// 		WebGenerator gen;
// 		gen.PushGraph("graphs/Aoriginal.csv");
// 		gen.PushGraph("graphs/Asmoothed.csv");
// 		gen.PushGraph("graphs/Aderivative.csv", GraphType::PositiveNegative);
// 		gen.PushGraph("graphs/AsmoothedDerivative.csv", GraphType::PositiveNegative);
// 		gen.Generate("D:\\ProjectHillside\\web\\Agenerated.html");
	}
}

//////////////////////////////////////////////////////////////////////////
bool Trade::PreSellUpdate(double current)
{
	double smoothedDerivate = 0.0;
	auto derivativeCurve = gModel.m_SmoothedDerivativeCurve.CreateSubcurveFromIndex(m_CreatedDataIndex);
	auto& derivative = derivativeCurve.m_Data;

	for (unsigned i = 0; i < derivative.size(); ++i)
	{
		smoothedDerivate = SmoothValue(smoothedDerivate, derivative[i], gSellSmoothWeight);
	}

	if (smoothedDerivate < 0)
	{
		gMarket.Sell(*this);
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// ORIGINAL UPDATES
////////////////////////////////////////////////////////////////////////////
//void Trade::PreBuyUpdate(double current)
//{
//	++m_Samples;
//	auto diff = current - m_PrevPrice;
//
//	m_SmoothedDerivate = SmoothValue(m_SmoothedDerivate, diff, gBuySmoothWeight);
//
//	if (m_Samples > 10 && m_SmoothedDerivate > 0)
//	{
//		gMarket.Buy(1.0, *this);
//	}
//}
////
//////////////////////////////////////////////////////////////////////////////
//bool Trade::PreSellUpdate(double current)
//{
//	auto diff = current - m_PrevPrice;
//
//	m_SmoothedDerivate = SmoothValue(m_SmoothedDerivate, diff, gSellSmoothWeight);
//	if (m_SmoothedDerivate < 0)
//	{
//		gMarket.Sell(*this);
//		return false;
//	}
//
//	return true;
//}