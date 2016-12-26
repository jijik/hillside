#pragma once

#include <vector>

#include "Trade.h"
#include "Curve.h"

struct Market
{
	static const double ratio;

	Market();

	std::vector<Trade*> m_CurrentTrades;
	std::vector<Trade*> m_TradeHistory;

	Trade*	Buy(double amount);
	void		Sell(Trade*);

	bool		AdvanceTime();	//true if there is more

	double	GetCurrentPrice();
	double	GetNextPrice();

	void		SaveTrades(const char* path);

	unsigned	m_CurrentDataIndex = 0;

	Curve			m_OriginalCurve;
};

