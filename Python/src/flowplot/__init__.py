from importlib import resources

from . import _flowplot
from ._flowplot import Plot, getCompleteJson, get_complete_json


def _default_font_resource():
    return resources.files(__package__).joinpath("fonts", "Inter.ttf")


def plot(path):
    plot_obj = _flowplot.plot(path)
    with resources.as_file(_default_font_resource()) as font_path:
        plot_obj.set_default_font(str(font_path))
    return plot_obj

__all__ = [
    "Plot",
    "getCompleteJson",
    "get_complete_json",
    "plot",
]
