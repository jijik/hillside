#pragma once

#include <vector>

#include "Trade.h"
#include "Curve.h"

struct Market
{
	static const double ratio;

	Market();

	std::vector<Trade*> m_TradeHistory;

	void		Init(double from, double to);

	void		Buy(double amount, Trade&);
	void		Sell(Trade&);

	bool		AdvanceTime();	//true if there is more

	double	GetCurrentPrice();

	void		SaveTrades(const char* path);

	void		Reset();

	unsigned	m_CurrentDataIndex = 0;

	Curve			m_OriginalCurve; //current segment
	Curve			m_OrigianlFullCurve;

	double		m_From;
	double		m_To;
};

