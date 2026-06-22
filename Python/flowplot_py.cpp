#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "FlowPlot_Mega.hpp"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace py = pybind11;

namespace
{
	py::tuple colorToTuple(const FlowPlot::Color& color)
	{
		return py::make_tuple(color.r, color.g, color.b, color.a);
	}

	py::tuple pointToTuple(const FlowPlot::PointF& point)
	{
		return py::make_tuple(point.x, point.y);
	}

	py::tuple rectToTuple(const FlowPlot::RectF& rect)
	{
		return py::make_tuple(rect.x, rect.y, rect.w, rect.h);
	}

	const char* horizontalAlignName(FlowPlot::HorizontalAlign align) noexcept
	{
		switch (align)
		{
		case FlowPlot::HorizontalAlign::Left:
			return "left";
		case FlowPlot::HorizontalAlign::Center:
			return "center";
		case FlowPlot::HorizontalAlign::Right:
			return "right";
		}
		return "left";
	}

	const char* verticalAlignName(FlowPlot::VerticalAlign align) noexcept
	{
		switch (align)
		{
		case FlowPlot::VerticalAlign::Top:
			return "top";
		case FlowPlot::VerticalAlign::Middle:
			return "middle";
		case FlowPlot::VerticalAlign::Bottom:
			return "bottom";
		}
		return "top";
	}

	const char* markerShapeName(FlowPlot::MarkerShape shape) noexcept
	{
		switch (shape)
		{
		case FlowPlot::MarkerShape::Circle:
			return "circle";
		case FlowPlot::MarkerShape::Square:
			return "square";
		case FlowPlot::MarkerShape::Diamond:
			return "diamond";
		case FlowPlot::MarkerShape::Triangle:
			return "triangle";
		}
		return "circle";
	}

	const char* lineCapName(FlowPlot::LineCap cap) noexcept
	{
		switch (cap)
		{
		case FlowPlot::LineCap::Butt:
			return "butt";
		case FlowPlot::LineCap::Square:
			return "square";
		case FlowPlot::LineCap::Round:
			return "round";
		}
		return "butt";
	}

	const char* lineJoinName(FlowPlot::LineJoin join) noexcept
	{
		switch (join)
		{
		case FlowPlot::LineJoin::Miter:
			return "miter";
		case FlowPlot::LineJoin::Bevel:
			return "bevel";
		case FlowPlot::LineJoin::Round:
			return "round";
		}
		return "bevel";
	}

	py::list pointsToList(const std::vector<FlowPlot::PointF>& points)
	{
		py::list out;
		for (const FlowPlot::PointF& point : points)
			out.append(pointToTuple(point));
		return out;
	}

	py::list colorsToList(const std::vector<FlowPlot::Color>& colors)
	{
		py::list out;
		for (const FlowPlot::Color& color : colors)
			out.append(colorToTuple(color));
		return out;
	}

	py::list floatsToList(const std::vector<float>& values)
	{
		py::list out;
		for (float value : values)
			out.append(value);
		return out;
	}

	py::dict commandToDict(const FlowPlot::BoxCommand& command)
	{
		py::dict out;
		out["type"] = "box";
		out["rect"] = rectToTuple(command.rect);
		out["fill"] = colorToTuple(command.fill);
		out["stroke"] = colorToTuple(command.stroke);
		out["stroke_width"] = command.strokeWidth;
		return out;
	}

	py::dict commandToDict(const FlowPlot::PolylineCommand& command)
	{
		py::dict out;
		out["type"] = "polyline";
		out["points"] = pointsToList(command.points);
		out["color"] = colorToTuple(command.color);
		out["width"] = command.width;
		out["cap"] = lineCapName(command.cap);
		out["join"] = lineJoinName(command.join);
		return out;
	}

	py::dict commandToDict(const FlowPlot::TextCommand& command)
	{
		py::dict out;
		out["type"] = "text";
		out["box"] = rectToTuple(command.box);
		out["text"] = command.text;
		out["color"] = colorToTuple(command.color);
		out["font_family"] = command.fontFamily;
		out["font_size"] = command.fontSize;
		out["font_weight"] = command.fontWeight;
		out["font_style"] = FlowInternal::fontStyleName(command.fontStyle);
		out["h_align"] = horizontalAlignName(command.hAlign);
		out["v_align"] = verticalAlignName(command.vAlign);
		out["clip_to_box"] = command.clipToBox;
		return out;
	}

	py::dict commandToDict(const FlowPlot::MarkersCommand& command)
	{
		py::dict out;
		out["type"] = "markers";
		out["shape"] = markerShapeName(command.shape);
		out["positions"] = pointsToList(command.positions);
		out["fills"] = colorsToList(command.fills);
		out["stroke"] = colorToTuple(command.stroke);
		out["sizes"] = floatsToList(command.sizes);
		out["stroke_width"] = command.strokeWidth;
		return out;
	}

	py::dict commandToDict(const FlowPlot::PushClipRectCommand& command)
	{
		py::dict out;
		out["type"] = "push_clip_rect";
		out["rect"] = rectToTuple(command.rect);
		return out;
	}

	py::dict commandToDict(const FlowPlot::PopClipRectCommand&)
	{
		py::dict out;
		out["type"] = "pop_clip_rect";
		return out;
	}

	py::dict renderPlotToDict(const FlowPlot::RenderPlot& plot)
	{
		py::list commands;
		for (const FlowPlot::RenderCommand& command : plot.commands)
		{
			commands.append(std::visit(
				[](const auto& concreteCommand)
				{
					return commandToDict(concreteCommand);
				},
				command));
		}

		py::dict out;
		out["width"] = plot.width;
		out["height"] = plot.height;
		out["background"] = colorToTuple(plot.background);
		out["commands"] = std::move(commands);
		return out;
	}

	class PyPlot
	{
	public:
		explicit PyPlot(const std::string& path)
			: builder_(FlowPlot::makePlot(path))
		{
		}

		PyPlot& set(const std::string& property, py::object value)
		{
			if (py::isinstance<py::bool_>(value))
				builder_.set(property, value.cast<bool>());
			else if (py::isinstance<py::int_>(value))
				builder_.set(property, value.cast<int>());
			else if (py::isinstance<py::float_>(value))
				builder_.set(property, value.cast<double>());
			else if (py::isinstance<py::str>(value))
				builder_.set(property, value.cast<std::string>());
			else if (value.is_none())
				throw py::type_error("set() does not accept None; use set_json_raw(property, \"null\")");
			else
				throw py::type_error("set() value must be bool, int, float, or str");

			return *this;
		}

		PyPlot& setJsonRaw(const std::string& property, const std::string& jsonText)
		{
			builder_.setJsonRaw(property, jsonText);
			return *this;
		}

		PyPlot& setDefaultFont(const std::string& fontPath)
		{
			textEngine_.emplace(std::filesystem::path(fontPath));
			builder_.useTextEngine(*textEngine_);
			return *this;
		}

		PyPlot& withData(const std::string& datasetField, py::object data)
		{
			if (py::isinstance<py::array>(data))
				return withNumpyData(datasetField, data);

			if (py::isinstance<py::sequence>(data) && !py::isinstance<py::str>(data))
				return withSequenceData(datasetField, data.cast<py::sequence>());

			throw py::type_error("with_data() expects a 1D sequence or NumPy array");
		}

		py::dict getCommands() const
		{
			prepareTextEngine("get_commands()");
			return renderPlotToDict(builder_.getCommands());
		}

		void writePng(const std::string& outputPath) const
		{
			prepareTextEngine("write_png()");
			builder_.writePng(outputPath);
		}

	private:
		void prepareTextEngine(const char* caller) const
		{
			if (!textEngine_.has_value())
			{
				throw py::value_error(
					std::string(caller)
					+ " requires a default font in Python; call set_default_font(path_to_ttf) first");
			}

			builder_.registerTemplateFonts(*textEngine_);
		}

		PyPlot& withNumpyData(const std::string& datasetField, py::object data)
		{
			using DoubleArray = py::array_t<double, py::array::c_style | py::array::forcecast>;

			DoubleArray array = DoubleArray::ensure(data);
			if (!array)
				throw py::type_error("with_data() NumPy arrays must contain numeric data");

			py::buffer_info info = array.request();
			if (info.ndim != 1)
				throw py::value_error("with_data() NumPy arrays must be 1D");

			keptArrays_.push_back(array);
			builder_.withData(
				datasetField,
				std::span<const double>(
					static_cast<const double*>(info.ptr),
					static_cast<std::size_t>(info.shape[0])));
			return *this;
		}

		PyPlot& withSequenceData(const std::string& datasetField, py::sequence sequence)
		{
			if (sequence.empty())
			{
				ownedDoubles_.emplace_back();
				builder_.withData(datasetField, ownedDoubles_.back());
				return *this;
			}

			py::object first = sequence[0];
			if (py::isinstance<py::str>(first))
				return withStringSequenceData(datasetField, sequence);

			if (py::isinstance<py::bool_>(first))
				throw py::type_error("with_data() boolean sequences are not supported yet");

			if (py::isinstance<py::int_>(first) || py::isinstance<py::float_>(first))
				return withNumericSequenceData(datasetField, sequence);

			throw py::type_error("with_data() sequence items must be numbers or strings");
		}

		PyPlot& withNumericSequenceData(const std::string& datasetField, py::sequence sequence)
		{
			std::vector<double>& values = ownedDoubles_.emplace_back();
			values.reserve(static_cast<std::size_t>(sequence.size()));

			for (py::handle item : sequence)
			{
				if (py::isinstance<py::bool_>(item) || !(py::isinstance<py::int_>(item) || py::isinstance<py::float_>(item)))
					throw py::type_error("numeric data sequences cannot contain non-numeric values");

				values.push_back(py::cast<double>(item));
			}

			builder_.withData(datasetField, values);
			return *this;
		}

		PyPlot& withStringSequenceData(const std::string& datasetField, py::sequence sequence)
		{
			std::vector<std::string>& values = ownedStrings_.emplace_back();
			values.reserve(static_cast<std::size_t>(sequence.size()));

			for (py::handle item : sequence)
			{
				if (!py::isinstance<py::str>(item))
					throw py::type_error("string data sequences cannot contain non-string values");

				values.push_back(py::cast<std::string>(item));
			}

			builder_.withData(datasetField, values);
			return *this;
		}

		FlowPlot::PlotBuilder builder_;
		mutable std::optional<FlowPlot::StbTextEngine> textEngine_{};
		std::vector<py::array> keptArrays_{};
		std::vector<std::vector<double>> ownedDoubles_{};
		std::vector<std::vector<std::string>> ownedStrings_{};
	};

	PyPlot plot(const std::string& path)
	{
		return PyPlot(path);
	}
} // namespace

PYBIND11_MODULE(_flowplot, module)
{
	module.doc() = "Python bindings for FlowPlot";

	py::class_<PyPlot>(module, "Plot")
		.def("set", &PyPlot::set, py::arg("property"), py::arg("value"), py::return_value_policy::reference_internal)
		.def("set_json_raw", &PyPlot::setJsonRaw, py::arg("property"), py::arg("json"), py::return_value_policy::reference_internal)
		.def("setJsonRaw", &PyPlot::setJsonRaw, py::arg("property"), py::arg("json"), py::return_value_policy::reference_internal)
		.def("set_default_font", &PyPlot::setDefaultFont, py::arg("font_path"), py::return_value_policy::reference_internal)
		.def("setDefaultFont", &PyPlot::setDefaultFont, py::arg("font_path"), py::return_value_policy::reference_internal)
		.def("with_data", &PyPlot::withData, py::arg("dataset_field"), py::arg("data"), py::return_value_policy::reference_internal)
		.def("withData", &PyPlot::withData, py::arg("dataset_field"), py::arg("data"), py::return_value_policy::reference_internal)
		.def("get_commands", &PyPlot::getCommands)
		.def("getCommands", &PyPlot::getCommands)
		.def("write_png", &PyPlot::writePng, py::arg("output_path"))
		.def("writePng", &PyPlot::writePng, py::arg("output_path"));

	module.def("plot", &plot, py::arg("path"));
	module.def(
		"get_complete_json",
		[](const std::string& templateJson, bool pretty)
		{
			return FlowPlot::getCompleteJson(templateJson, pretty);
		},
		py::arg("template_json"),
		py::arg("pretty") = true);
	module.def(
		"getCompleteJson",
		[](const std::string& templateJson, bool pretty)
		{
			return FlowPlot::getCompleteJson(templateJson, pretty);
		},
		py::arg("template_json"),
		py::arg("pretty") = true);
}
