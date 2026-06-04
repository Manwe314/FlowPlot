import flowplot

plot = flowplot.plot("./ScatterTemplate")
plot.with_data("main.x", [3, 5, 6, 8, 4, 3])
plot.with_data("main.y", [6, 3, 3, 9, 9, 12])
plot.write_png("pythonPlot.png")