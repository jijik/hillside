#include "Trade.h"
#include "Market.h"

//////////////////////////////////////////////////////////////////////////
Trade::Trade(double amount, double boughtAtPrice, unsigned dataIndex)
	: m_BoughtAtPrice(boughtAtPrice)
	, m_Amount(amount)
	, m_BoughtDataIndex(dataIndex)
{
	m_Zip = boughtAtPrice * (1.0 + (1.0 - Market::ratio));
}

