#pragma once

struct Trade
{
	Trade(double amount, double boughtAtPrice, unsigned dataIndex);

	double		m_BoughtAtPrice;
	double		m_Amount;
	double		m_Zip;		//price we must sell at not to have negative profit
	unsigned	m_BoughtDataIndex;
	unsigned	m_SoldDataIndex;

	double		m_Profit; //calculated when sold
};