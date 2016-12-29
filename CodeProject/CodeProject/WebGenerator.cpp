#include "WebGenerator.h"
#include "genv.h"
#include <sstream>

//////////////////////////////////////////////////////////////////////////
void WebGenerator::PushGraph(const char* path, GraphType::Type type)
{
	m_Graphs.emplace_back(type, path);
}

//////////////////////////////////////////////////////////////////////////
void WebGenerator::PushCurrentGraph()
{
	std::stringstream ss; 
	AppendSuffix(ss, "");
	m_GraphSegment.push_back(ss.str().erase(0,1));
}

//////////////////////////////////////////////////////////////////////////
void WebGenerator::Generate(const char* path)
{
	m_fs.open(path);

	WriteLine("<!DOCTYPE html>");
	WriteLine("<html>");

	GenerateHeader();
//	GenerateBody();
	GenerateBody_New();

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
		m_fs << "labels : [\"X\", \"c1\", \"c2\"],\n";
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
void WebGenerator::GenerateBody_New()
{
	WriteLine("<body>");

	m_fs << "TOTAL: " << gTotalProfit << "<br/><br/>\n";

	std::string path = "D:\\ProjectHillside\\web\\";

	for (auto& graphSufix : m_GraphSegment)
	{
		std::string subFolder = "graphs/";
		std::string bestFileName = subFolder + "best_" + graphSufix + ".txt";
		std::string annotationFileName = subFolder + "annotations_" + graphSufix + ".js";
		std::string idealCurveFileName = subFolder + "idealCurve_" + graphSufix + ".csv";
		std::string graphId(std::string("g") + graphSufix);

		std::ifstream bestSrc(path + bestFileName);
		std::stringstream file;
		file << bestSrc.rdbuf(); bestSrc.close();

		m_fs << file.str();

		m_fs << "<div id=\"" << graphId << "\" style=\"width:90%;\"></div><br/>\n";

		m_fs << "<script type=\"text/javascript\" src=\"" << annotationFileName << "\"></script>" << "\n";

		m_fs << "<script type=\"text/javascript\">\n";
		m_fs << graphId << " = new Dygraph(document.getElementById(\"" << graphId << "\"), \"" << idealCurveFileName << "\", {\n";
		m_fs << "highlightCircleSize: 2, highlightSeriesOpts : { strokeWidth: 1, strokeBorderWidth : 1, highlightCircleSize : 2 },\n";
		m_fs << "labels : [\"X\", \"c1\", \"c2\"],\n";
		m_fs << "animatedZooms : true,\n";
		m_fs << "});\n\n";

		m_fs << graphId << ".ready(gAnnotation_" << graphSufix;
		m_fs << "(" << graphId << "));\n";

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
