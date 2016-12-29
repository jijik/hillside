#pragma once

#include <functional>
#include <chrono>
#include <random>

class Rand_double
{
public:
	Rand_double(double low, double high)
		:r(std::bind(std::uniform_real_distribution<>(low, high), std::default_random_engine(std::random_device()()))) {}


	double operator()() { return r(); }

private:
	std::function<double()> r;
};

struct Combinator
{
	struct Variable
	{
		double* m_Variable;
		double	m_MinVal;
		double	m_MaxVal;
		double	m_Increment;
	};

	std::vector<Variable> m_Variables;
	std::vector<Rand_double> m_Randoms;

	void	AddVariable(double& var, double min, double max, double increment)
	{
		m_Variables.push_back({ &var, min, max, increment });
	}

	template <typename Func>
	void	Combine(Func func, double randomSearchDurationSecond = 0.0)
	{
		unsigned i = 0;

		unsigned combinations = 1;

		for (auto& var : m_Variables)
		{
			*var.m_Variable = var.m_MinVal;

			combinations *= unsigned((var.m_MaxVal - var.m_MinVal) / var.m_Increment) + 1;

			m_Randoms.emplace_back(var.m_MinVal, var.m_MaxVal);
		}

		unsigned fraction = combinations / 100;	//1%
		if (fraction == 0) fraction = 1;

		std::function<bool(unsigned)> carry = [&](unsigned i)
		{
			auto& v = m_Variables[i];
			*v.m_Variable += v.m_Increment;
			if (*v.m_Variable > v.m_MaxVal)
			{
				*v.m_Variable = v.m_MinVal;
				if (i < m_Variables.size() - 1)
				{
					return carry(i + 1);
				}
				else 
				{
					return true;
				}
			}
			return false;
		};

		auto randomValues = [&]() 
		{
			for (unsigned i = 0; i < m_Variables.size(); ++i)
			{
				*m_Variables[i].m_Variable = m_Randoms[i]();
			}
			return false;
		};

		unsigned counter = 0;

		if (randomSearchDurationSecond == 0.0)	// iterate combinations
		{
			while (true)
			{
				func();
				auto shouldbreak = carry(0);
				if (shouldbreak) break;

				++counter;
				if (counter % fraction == 0)
				{
					std::cout << counter * 100 / combinations << "%\n";
				}
			}
		}
		else //sample
		{
			auto start = std::chrono::steady_clock::now();

			while (true)
			{
				func();
				randomValues();

				auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start);
				if (duration.count() > randomSearchDurationSecond)
				{
					std::cout << "SearchTime is up...\n";
					break;
				}
			}
		}
	}
};