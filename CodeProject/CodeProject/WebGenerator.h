#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

namespace GraphType
{
	enum Type { None, Annotated, PositiveNegative };
}

struct WebGenerator
{
	void PushGraph(const char*, GraphType::Type type = GraphType::None);

	void Generate(const char* path);

private:
	void	GenerateHeader();
	void	GenerateBody();

	void	WriteLine(const char*);

	std::ofstream m_fs;


	std::vector<std::pair<GraphType::Type,std::string>> m_Graphs;
};
