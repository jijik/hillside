#include "Model.h"
#include "genv.h"

//////////////////////////////////////////////////////////////////////////
void Model::UpdateTrades()
{
	auto currentPrice = gMarket.GetCurrentPrice();

	auto originalSize = m_CurrentTrades.size();

	m_OriginalCurve.m_Data.push_back(currentPrice);

 	if (gMarket.m_CurrentDataIndex <= 15)
 	{
 		return;	// we need a bit of data 
 	}

 	m_SmoothedCurve = m_OriginalCurve.CreateMovingAverage(15);
 	m_DerivativeCurve = m_SmoothedCurve.CreateDerivate();
 	m_SmoothedDerivativeCurve = m_DerivativeCurve.CreateMovingAverage(15);

	for (auto it = m_CurrentTrades.begin(); it != m_CurrentTrades.end(); /*noincrement*/)
	{
		auto& trade = **it;
		bool wantsMore = trade.Update(currentPrice);
		if (wantsMore)
		{
			++it;
		}
		else
		{
			it = m_CurrentTrades.erase(it);
		}
	}

	// check if some trade has ended
	if (m_CurrentTrades.empty() || originalSize != m_CurrentTrades.size())
	{
		Trade* trade = new Trade;
		trade->m_PrevPrice = currentPrice;
		trade->m_CreatedDataIndex = gMarket.m_CurrentDataIndex;
		m_CurrentTrades.push_back(trade);
	}
}

//////////////////////////////////////////////////////////////////////////
void Model::Reset()
{
	for (auto* t : m_CurrentTrades)
	{
		delete t;
	}
	m_CurrentTrades.clear();

	m_OriginalCurve.Clear();
	m_SmoothedCurve.Clear();
	m_DerivativeCurve.Clear();
	m_SmoothedDerivativeCurve.Clear();
}
