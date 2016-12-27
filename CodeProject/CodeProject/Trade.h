#pragma once

#include "Curve.h"

struct Trade
{
	void			Buy(double amount, double boughtAtPrice, unsigned dataIndex);
	bool			Update(double current, double next); // return true if wants more updates

	double		m_BoughtAtPrice;
	double		m_Amount;
	double		m_Zip;		//price we must sell at not to have negative profit
	unsigned	m_BoughtDataIndex;
	unsigned	m_SoldDataIndex;

	bool			m_Bought = false;

	double		m_Profit; //calculated when sold

	double		m_PrevPrice;

	unsigned	m_Samples = 0;
	double		m_SmoothedDerivate = 0.0;

	Curve			m_Curve;

	void			PreBuyUpdate(double current);
	bool			PreSellUpdate(double current); // return true if wants more update
};