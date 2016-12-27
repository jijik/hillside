#include "Trade.h"
#include "Market.h"
#include "genv.h"

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
bool Trade::Update(double current, double next)
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
// 	m_Curve.m_Data.push_back(current);
// 
// 	auto smoothedCurve = m_Curve.CreateMovingAverage(15);
// 	auto derivate = smoothedCurve.CreateDerivate();
// 
// 	double smoothedDerivate = 0.0;
// 	for (unsigned i = 0; i < derivate.m_Data.size() - 1; ++i)
// 	{
// 		auto diff = derivate.m_Data[i + 1] - derivate.m_Data[i];
// 		smoothedDerivate = SmoothValue(smoothedDerivate, diff, gBuySmoothWeight);
// 	}

	++m_Samples;
	auto diff = current - m_PrevPrice;

	m_SmoothedDerivate = SmoothValue(m_SmoothedDerivate, diff, gBuySmoothWeight);

	if (m_Samples > 10 && m_SmoothedDerivate > 0)
	{
		gMarket.Buy(1.0, *this);
	}
}

//////////////////////////////////////////////////////////////////////////
bool Trade::PreSellUpdate(double current)
{
// 	m_Curve.m_Data.push_back(current);
// 
// 	auto smoothedCurve = m_Curve.CreateMovingAverage(15);
// 	auto derivate = smoothedCurve.CreateDerivate();
// 
// 	double smoothedDerivate = 0.0;
// 	for (unsigned i = 0; i < derivate.m_Data.size() - 1; ++i)
// 	{
// 		auto diff = derivate.m_Data[i + 1] - derivate.m_Data[i];
// 		smoothedDerivate = SmoothValue(smoothedDerivate, diff, gBuySmoothWeight);
// 	}
// 
// 	if (smoothedDerivate < 0)
// 	{
// 		gMarket.Sell(*this);
// 		return false;
// 	}
// 	return true;

	auto diff = current - m_PrevPrice;

	m_SmoothedDerivate = SmoothValue(m_SmoothedDerivate, diff, gSellSmoothWeight);
	if (m_SmoothedDerivate < 0)
	{
		gMarket.Sell(*this);
		return false;
	}

	return true;
	 }