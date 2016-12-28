#pragma once

#include <vector>
#include "Curve.h"

struct Trade;

struct Model
{
	void	UpdateTrades();

	void	Reset();

	Curve m_OriginalCurve;
	Curve	m_SmoothedCurve;
	Curve	m_DerivativeCurve;
	Curve	m_SmoothedDerivativeCurve;

	std::vector<Trade*> m_CurrentTrades;
};

