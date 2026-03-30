#define FLOW_PLOT_RENDERER
#define FLOW_PLOT_IMPLEMENTATION
#define FLOW_PLOT_DEFAULT_FONT_PATH "./FacultyGlyphic-Regular.ttf"
#include "FlowPlot_Mega.hpp"

#include<vector>

int main()
{
	std::vector<int> x{1, 2, 3, 4, 5};
	std::vector<int> y{3, 4, 1, 4, 6};

	FlowPlot::plot("./ScatterTemplate.json").withData("main.x", x).withData("main.y", y).writePng("./myPng.png");
	return 0;
}
