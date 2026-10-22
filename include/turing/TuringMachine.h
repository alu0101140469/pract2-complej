/**
 * @file TuringMachine.h
 * @brief Simulador de máquinas de Turing deterministas con múltiples cintas.
 * Proyecto: Complejidad Computacional - Práctica 2
 * Curso: 2026/27
 * @author Daniel Palenzuela Álvarez alu0101140469
 */
 
#ifndef PRACTICA2_TURING_MACHINE_H
#define PRACTICA2_TURING_MACHINE_H

#include "turing/MachineDefinition.h"
#include "turing/Tape.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace turing {

/** @brief Resultado de una ejecución de la máquina. */
struct RunResult {
    /** @brief Verdadero si la ejecución se detuvo porque no existía ninguna transición. */
    bool halted = false;

    /** @brief Verdadero únicamente cuando la máquina se detuvo en un estado de aceptación. */
    bool accepted = false;

    /** @brief Verdadero si un límite de pasos configurado explícitamente interrumpió la ejecución. */
    bool stepLimitReached = false;

    /** @brief Número de aplicaciones de transición completadas. */
    std::uint64_t steps = 0;

    /** @brief Estado en el que la ejecución se detuvo o fue interrumpida. */
    std::string finalState;
};

/** @brief Ejecuta una definición validada mediante pasos simultáneos de lectura/escritura/movimiento. */
class TuringMachine {
public:
    /** @brief Crea un simulador a partir de una definición de máquina validada. */
    explicit TuringMachine(MachineDefinition definition);

    /** @brief Reinicia el estado, los cabezales y todas las cintas para una nueva cadena de entrada. */
    void reset(const std::string& input);

    /**
     * @brief Ejecuta hasta que no exista ninguna transición o hasta alcanzar un límite opcional.
     * @param maxSteps Límite de seguridad opcional. Sin él, la detención depende únicamente de que falten transiciones.
     */
    RunResult run(std::optional<std::uint64_t> maxSteps = std::nullopt);

    /** @brief Devuelve el nombre del estado actual. */
    const std::string& currentState() const;

    /** @brief Devuelve todas las cintas, incluidas sus posiciones actuales de los cabezales. */
    const std::vector<Tape>& tapes() const;

    /** @brief Devuelve la definición inmutable de la máquina. */
    const MachineDefinition& definition() const;

private:
    bool performStep();
    const Transition* currentTransition() const;

    MachineDefinition definition_;
    std::string currentState_;
    std::vector<Tape> tapes_;
    bool halted_ = false;
};

}

#endif
