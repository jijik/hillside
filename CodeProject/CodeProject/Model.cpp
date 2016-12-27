#include "Model.h"
#include "genv.h"

//////////////////////////////////////////////////////////////////////////
void Model::UpdateTrades()
{
	auto currentPrice = gMarket.GetCurrentPrice();
	auto nextPrice = gMarket.GetNextPrice();

	auto originalSize = m_CurrentTrades.size();

	for (auto it = m_CurrentTrades.begin(); it != m_CurrentTrades.end(); /*noincrement*/)
	{
		auto& trade = **it;
		bool wantsMore = trade.Update(currentPrice, nextPrice);
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
}
