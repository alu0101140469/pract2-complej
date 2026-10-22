#!/usr/bin/env python3
from collections import defaultdict
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]
MACHINES = {
    ROOT / "machines" / "MT1_lenguaje.txt": "MT1_lenguaje",
    ROOT / "machines" / "MT2_conteo.txt": "MT2_conteo",
}
OUTPUT = ROOT / "docs" / "graphs"


def clean_lines(path: Path) -> list[str]:
    lines = []
    for raw in path.read_text(encoding="utf-8").splitlines():
        value = raw.strip()
        if value and not value.startswith("#"):
            lines.append(value)
    return lines


def quote(value: str) -> str:
    return '"' + value.replace('\\', '\\\\').replace('"', '\\"').replace('\n', '\\n') + '"'


def graph_for_config(config_path: Path, graph_name: str) -> str:
    lines = clean_lines(config_path)
    states = lines[0].split()
    final_states = lines[5].split()
    tape_count = int(lines[6].split()[0])
    grouped = defaultdict(list)
    identity_reads = defaultdict(set)

    for row in lines[7:]:
        tokens = row.split()
        source = tokens[0]
        reads = tokens[1:1 + tape_count]
        target = tokens[1 + tape_count]
        writes = tokens[2 + tape_count:2 + 2 * tape_count]
        moves = tokens[2 + 2 * tape_count:2 + 3 * tape_count]
        move_label = "".join(moves)
        if reads == writes:
            identity_reads[(source, target, move_label)].add(" ".join(reads))
        else:
            if tape_count == 1:
                label = f"{reads[0]}/{writes[0]}, {move_label}"
            else:
                label = f"({' '.join(reads)})/({' '.join(writes)}), {move_label}"
            grouped[(source, target)].append(label)

    for (source, target, moves), read_vectors in identity_reads.items():
        reads = sorted(read_vectors)
        if tape_count == 1:
            label = f"{','.join(reads)}: keep, {moves}"
        else:
            label = f"({'; '.join(reads)}): keep, {moves}"
        grouped[(source, target)].append(label)

    rank_direction = "TB" if graph_name == "MT2_conteo" else "LR"
    lines_out = [
        "// Generated automatically from " + config_path.relative_to(ROOT).as_posix(),
        f"digraph {graph_name} {{",
        f'  graph [rankdir={rank_direction}, splines=true, overlap=false, outputorder=edgesfirst, pad=0.25, '
        'nodesep=0.65, ranksep=0.85, fontname="Arial", fontsize=10, labelloc="t", label=' +
        quote(graph_name.replace('_', ' ') + "\nLabels: read/write, movement; 'keep' means write the same symbol.") + "];",
        '  node [shape=circle, fontname="Arial", fontsize=10, margin="0.10,0.07"];',
        '  edge [fontname="Arial", fontsize=9, arrowsize=0.7];',
        '  __start [shape=point, label="", width=0.12];',
        '  __start -> ' + quote(states[0]) + ';',
    ]
    for state in states:
        shape = "doublecircle" if state in final_states else "circle"
        lines_out.append(f"  {quote(state)} [shape={shape}, label={quote(state)}];")
    for (source, target), labels in sorted(grouped.items()):
        label = "\n".join(sorted(labels))
        lines_out.append(f"  {quote(source)} -> {quote(target)} [label={quote(label)}];")
    lines_out.append("}")
    return "\n".join(lines_out) + "\n"


def main() -> None:
    OUTPUT.mkdir(parents=True, exist_ok=True)
    for config, name in MACHINES.items():
        dot_text = graph_for_config(config, name)
        dot_path = OUTPUT / f"{name}.dot"
        dot_path.write_text(dot_text, encoding="utf-8")
        for extension in ("svg", "png"):
            command = ["dot", f"-T{extension}"]
            if extension == "png":
                command.append("-Gdpi=130")
            command.extend([str(dot_path), "-o", str(OUTPUT / f"{name}.{extension}")])
            result = subprocess.run(
                command,
                stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, check=False,
            )
            if result.returncode != 0:
                raise RuntimeError(f"Graphviz fallo al generar {name}.{extension}: {result.stderr}")
        print(f"Generados: {name}.dot, {name}.svg, {name}.png")


if __name__ == "__main__":
    main()
