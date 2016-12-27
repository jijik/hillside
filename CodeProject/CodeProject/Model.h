#pragma once

#include <vector>

struct Trade;

struct Model
{
	void	UpdateTrades();

	void	Reset();

	std::vector<Trade*> m_CurrentTrades;
};

