#!/usr/bin/env python3
"""
@brief Pruebas de la interfaz de línea de comandos del simulador de máquinas de Turing.
"""
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
PROGRAM = ROOT / "build" / "tm_simulator"


def run(args, expected_fragments, expected_code=0):
    completed = subprocess.run(
        [str(PROGRAM), *args], cwd=ROOT, text=True,
        stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=False,
    )
    if completed.returncode != expected_code:
        raise AssertionError(
            f"Codigo inesperado {completed.returncode} (esperado {expected_code}).\n"
            f"stdout:\n{completed.stdout}\nstderr:\n{completed.stderr}"
        )
    combined = completed.stdout + completed.stderr
    for fragment in expected_fragments:
        if fragment not in combined:
            raise AssertionError(f"No aparece {fragment!r} en la salida:\n{combined}")


if __name__ == "__main__":
    run(["-config", "machines/MT1_lenguaje.txt", "-in", "tests/inputs_mt1.txt"],
        ["Entrada: ab", "Resultado: ACEPTADA", "Entrada: aab", "Resultado: RECHAZADA"])
    run(["-config", "machines/MT2_conteo.txt", "-in", "tests/inputs_mt2.txt"],
        ["[1]1111.1111", "[0].11", "[1]1.0"])
    run(["-config", "machines/examples/MT_multicinta_demo.txt", "-in", "tests/inputs_multitape.txt"],
        ["Numero de cintas: 2", "Cinta 2: abb[a]"])
    run(["-config", "machines/MT1_lenguaje.txt", "-in", "tests/invalid_input.txt"],
        ["no puede aparecer en la entrada"], expected_code=1)
    print("Todas las pruebas de CLI han pasado.")
