#include "WebGenerator.h"

//////////////////////////////////////////////////////////////////////////
void WebGenerator::PushGraph(const char* path, GraphType::Type type)
{
	m_Graphs.emplace_back(type, path);
}

//////////////////////////////////////////////////////////////////////////
void WebGenerator::Generate(const char* path)
{
	m_fs.open(path);

	WriteLine("<!DOCTYPE html>");
	WriteLine("<html>");

	GenerateHeader();
	GenerateBody();

	WriteLine("</html>");

	m_fs.close();
}

//////////////////////////////////////////////////////////////////////////
void WebGenerator::GenerateHeader()
{
	WriteLine("<head>");

	std::ifstream headSrc("D:\\ProjectHillside\\web\\WebPieces\\head.txt");
	m_fs << headSrc.rdbuf();

	std::ifstream cssSrc("D:\\ProjectHillside\\web\\WebPieces\\css.txt");
	m_fs << cssSrc.rdbuf();

	WriteLine("</head>");
}

//////////////////////////////////////////////////////////////////////////
void WebGenerator::GenerateBody()
{
	WriteLine("<body>");

	std::string divId("divId");
	unsigned id = 0;

	WriteLine("<script type=\"text/javascript\" src=\"annotations.js\"></script>");

	for (auto& graphInfo : m_Graphs)
	{
		auto& graphPath = graphInfo.second;

		std::string generatedId = divId + std::to_string(id++);
		m_fs << "<div id=\"" << generatedId << "\" style=\"width:90%;\"></div>\n";
		m_fs << "<script type=\"text/javascript\">\n";
		m_fs << generatedId << " = new Dygraph(document.getElementById(\"" << generatedId << "\"), \"" << graphPath << "\", {\n";
		m_fs << "highlightCircleSize: 2, highlightSeriesOpts : { strokeWidth: 1, strokeBorderWidth : 1, highlightCircleSize : 2 },\n";
		m_fs << "labels : [\"X\", \"value\", \"target\"],\n";
		m_fs << "animatedZooms : true,\n";

		if (graphInfo.first == GraphType::PositiveNegative)
		{
			m_fs << "underlayCallback: function(canvas, area, g) {";
			m_fs << "var zero = g.toDomCoords(0, 0);";
			m_fs << "canvas.fillStyle = \"#ffeeee\"; canvas.fillRect(zero[0], zero[1], 999*area.w, area.h);";
			m_fs << "canvas.fillStyle = \"#eeffee\"; canvas.fillRect(zero[0], 0, area.w*99, zero[1]);}";
		}

		m_fs << "});\n";
		
		if (graphInfo.first == GraphType::Annotated)
		{
			m_fs << generatedId << ".ready(annFunc(" << generatedId << "));\n";
		}



		m_fs << "</script>\n";
	}

	WriteLine("</body>");
}

//////////////////////////////////////////////////////////////////////////
void WebGenerator::WriteLine(const char* text)
{
	m_fs << text << "\n";
}

//////////////////////////////////////////////////////////////////////////
