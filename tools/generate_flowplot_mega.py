#!/usr/bin/env python3
from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
import re
from typing import Callable


INCLUDE_RE = re.compile(r'^\s*#\s*include\s*([<"])([^>"]+)[>"](?:\s*//.*)?\s*$')

RAPIDJSON_SEEDS = (
    "rapidjson/document.h",
    "rapidjson/error/en.h",
    "rapidjson/istreamwrapper.h",
    "rapidjson/prettywriter.h",
    "rapidjson/stringbuffer.h",
    "rapidjson/writer.h",
)

STB_HEADER_NAMES = {
    "stb_truetype.h",
    "stb_image_write.h",
    "stb_rect_pack.h",
}


@dataclass(frozen=True)
class MegaVariant:
    output_name: str
    description: str
    inline_stb: bool
    inline_rapidjson: bool


VARIANTS = (
    MegaVariant(
        output_name="FlowPlot_Mega_Core.hpp",
        description="FlowPlot headers only (RapidJSON and stb remain external)",
        inline_stb=False,
        inline_rapidjson=False,
    ),
    MegaVariant(
        output_name="FlowPlot_Mega_Stb.hpp",
        description="FlowPlot + stb (RapidJSON remains external)",
        inline_stb=True,
        inline_rapidjson=False,
    ),
    MegaVariant(
        output_name="FlowPlot_Mega_Json.hpp",
        description="FlowPlot + used RapidJSON subset (stb remains external)",
        inline_stb=False,
        inline_rapidjson=True,
    ),
    MegaVariant(
        output_name="FlowPlot_Mega.hpp",
        description="FlowPlot + used RapidJSON subset + stb (fully self-contained)",
        inline_stb=True,
        inline_rapidjson=True,
    ),
)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def strip_pragma_once(text: str) -> str:
    lines = text.splitlines(keepends=True)
    if lines and lines[0].strip() == "#pragma once":
        lines = lines[1:]
        if lines and lines[0].strip() == "":
            lines = lines[1:]
    return "".join(lines)


def replace_once(text: str, needle: str, replacement: str, label: str) -> str:
    count = text.count(needle)
    if count != 1:
        raise RuntimeError(f"expected exactly one occurrence of {label!r}, found {count}")
    return text.replace(needle, replacement, 1)


def make_guard(filename: str) -> str:
    stem = re.sub(r"[^A-Za-z0-9]", "_", filename).upper()
    return f"{stem}_INCLUDED"


def parse_include(line: str) -> tuple[str, str] | None:
    match = INCLUDE_RE.match(line)
    if match is None:
        return None
    return match.group(1), match.group(2)


def strip_includes(text: str, should_strip: Callable[[str], bool]) -> str:
    out: list[str] = []
    for line in text.splitlines(keepends=True):
        parsed = parse_include(line)
        if parsed is not None and should_strip(parsed[1]):
            continue
        out.append(line)
    return "".join(out)


def is_within(path: Path, root: Path) -> bool:
    try:
        path.relative_to(root)
        return True
    except ValueError:
        return False


def build_resolver(extra_roots: tuple[Path, ...]) -> Callable[[Path, str, str], Path | None]:
    resolved_roots = tuple(root.resolve() for root in extra_roots)

    def resolve_include(current_file: Path, delimiter: str, target: str) -> Path | None:
        candidates: list[Path] = []
        if delimiter == '"':
            candidates.append((current_file.parent / target).resolve())

        for root in resolved_roots:
            candidates.append((root / target).resolve())

        for candidate in candidates:
            if candidate.is_file():
                return candidate

        return None

    return resolve_include


def inline_header_recursive(
    entry: Path,
    repo_root: Path,
    resolve_include: Callable[[Path, str, str], Path | None],
    should_inline: Callable[[Path], bool],
    visited: set[Path],
) -> str:
    path = entry.resolve()
    if path in visited:
        return ""

    visited.add(path)
    text = read_text(path)

    out: list[str] = []
    for line in text.splitlines(keepends=True):
        parsed = parse_include(line)
        if parsed is None:
            out.append(line)
            continue

        delimiter, target = parsed
        resolved = resolve_include(path, delimiter, target)
        if resolved is not None and should_inline(resolved):
            if resolved in visited:
                continue
            rel = resolved.relative_to(repo_root).as_posix()
            out.append(f"// ===== BEGIN inlined {rel} =====\n")
            out.append(
                inline_header_recursive(
                    resolved,
                    repo_root,
                    resolve_include,
                    should_inline,
                    visited,
                )
            )
            out.append(f"// ===== END inlined {rel} =====\n")
            continue

        out.append(line)

    return "".join(out)


def build_rapidjson_subset_block(repo_root: Path, flowplot_dir: Path) -> str:
    rapidjson_root = (flowplot_dir / "rapidjson").resolve()
    resolve_include = build_resolver((rapidjson_root,))
    should_inline = lambda path: is_within(path, rapidjson_root)

    visited: set[Path] = set()
    chunks: list[str] = []
    for seed in RAPIDJSON_SEEDS:
        seed_path = (flowplot_dir / seed).resolve()
        if seed_path in visited:
            continue

        rel = seed_path.relative_to(repo_root).as_posix()
        chunks.append(f"// ===== BEGIN inlined {rel} =====\n")
        chunks.append(
            inline_header_recursive(
                seed_path,
                repo_root,
                resolve_include,
                should_inline,
                visited,
            )
        )
        chunks.append(f"// ===== END inlined {rel} =====\n")

    return "".join(chunks)


def build_stb_inline_map(repo_root: Path, flowplot_dir: Path) -> dict[str, str]:
    resolve_include = build_resolver((flowplot_dir,))

    def should_inline(path: Path) -> bool:
        return path.parent == flowplot_dir and path.name in STB_HEADER_NAMES

    visited: set[Path] = set()
    inlined: dict[str, str] = {}

    for root_name in ("stb_truetype.h", "stb_image_write.h"):
        root_path = (flowplot_dir / root_name).resolve()
        rel = root_path.relative_to(repo_root).as_posix()
        body = inline_header_recursive(
            root_path,
            repo_root,
            resolve_include,
            should_inline,
            visited,
        )
        inlined[root_name] = (
            f"// ===== BEGIN inlined {rel} =====\n"
            f"{body}"
            f"// ===== END inlined {rel} ====="
        )

    return inlined


def build_flowplot_body(
    repo_root: Path,
    flowplot_dir: Path,
    *,
    inline_rapidjson: bool,
    inline_stb: bool,
    stb_inline_map: dict[str, str],
) -> str:
    flowplot_hpp = strip_pragma_once(read_text(flowplot_dir / "FlowPlot.hpp"))
    defaults_hpp = strip_pragma_once(read_text(flowplot_dir / "FlowPlot_Defaults.hpp"))
    internal_hpp = strip_pragma_once(read_text(flowplot_dir / "FlowPlot_Internal.hpp"))
    renderer_hpp = strip_pragma_once(read_text(flowplot_dir / "FlowPlot_Renderer.hpp"))

    internal_hpp = replace_once(
        internal_hpp,
        '#include "FlowPlot_Defaults.hpp"\n',
        "",
        "FlowPlot_Internal.hpp defaults include",
    )

    renderer_hpp = replace_once(
        renderer_hpp,
        '#ifndef FLOW_PLOT_HPP_INCLUDED\n#include "FlowPlot.hpp"\n#endif\n\n',
        "",
        "FlowPlot_Renderer.hpp FlowPlot include block",
    )

    if inline_stb:
        renderer_hpp = replace_once(
            renderer_hpp,
            '#include "stb_truetype.h"',
            stb_inline_map["stb_truetype.h"],
            "FlowPlot_Renderer.hpp stb_truetype include",
        )
        renderer_hpp = replace_once(
            renderer_hpp,
            '#include "stb_image_write.h"',
            stb_inline_map["stb_image_write.h"],
            "FlowPlot_Renderer.hpp stb_image_write include",
        )

    if inline_rapidjson:
        strip_rapidjson = lambda target: target.startswith("rapidjson/")
        flowplot_hpp = strip_includes(flowplot_hpp, strip_rapidjson)
        internal_hpp = strip_includes(internal_hpp, strip_rapidjson)

    flowplot_hpp = replace_once(
        flowplot_hpp,
        '#include "FlowPlot_Defaults.hpp"',
        "// ===== BEGIN inlined FlowPlot_Defaults.hpp =====\n"
        + defaults_hpp
        + "\n// ===== END inlined FlowPlot_Defaults.hpp =====",
        "FlowPlot.hpp defaults include",
    )
    flowplot_hpp = replace_once(
        flowplot_hpp,
        '#include "FlowPlot_Internal.hpp"',
        "// ===== BEGIN inlined FlowPlot_Internal.hpp =====\n"
        + internal_hpp
        + "\n// ===== END inlined FlowPlot_Internal.hpp =====",
        "FlowPlot.hpp internal include",
    )
    flowplot_hpp = replace_once(
        flowplot_hpp,
        '#include "FlowPlot_Renderer.hpp"',
        "// ===== BEGIN inlined FlowPlot_Renderer.hpp =====\n"
        + renderer_hpp
        + "\n// ===== END inlined FlowPlot_Renderer.hpp =====",
        "FlowPlot.hpp renderer include",
    )

    return flowplot_hpp


def write_variant(
    repo_root: Path,
    variant: MegaVariant,
    flowplot_body: str,
    rapidjson_block: str,
) -> None:
    output_path = repo_root / variant.output_name
    guard = make_guard(variant.output_name)

    header = (
        "#pragma once\n\n"
        f"#ifndef {guard}\n"
        f"#define {guard}\n\n"
        "// FlowPlot single-header amalgamation.\n"
        "// Generated by tools/generate_flowplot_mega.py.\n"
        f"// Variant: {variant.description}.\n\n"
    )

    if variant.inline_rapidjson:
        header += (
            "// ===== BEGIN inlined RapidJSON subset =====\n"
            f"{rapidjson_block}"
            "// ===== END inlined RapidJSON subset =====\n\n"
        )

    output = header + flowplot_body + f"\n#endif // {guard}\n"
    output_path.write_text(output, encoding="utf-8")


def main() -> None:
    repo_root = Path(__file__).resolve().parents[1]
    flowplot_dir = (repo_root / "FlowPlot").resolve()

    rapidjson_block = build_rapidjson_subset_block(repo_root, flowplot_dir)
    stb_inline_map = build_stb_inline_map(repo_root, flowplot_dir)

    for variant in VARIANTS:
        flowplot_body = build_flowplot_body(
            repo_root,
            flowplot_dir,
            inline_rapidjson=variant.inline_rapidjson,
            inline_stb=variant.inline_stb,
            stb_inline_map=stb_inline_map,
        )
        write_variant(repo_root, variant, flowplot_body, rapidjson_block)
        print(f"generated {variant.output_name}")


if __name__ == "__main__":
    main()
