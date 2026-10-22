/**
 * @file Transition.h
 * @brief Tipos que describen una transición determinista de una máquina de Turing multicinta.
 * Proyecto: Complejidad Computacional - Práctica 2
 * Curso: 2026/27
 * @author Daniel Palenzuela Álvarez alu0101140469
 */
 
#ifndef PRACTICA2_TURING_TRANSITION_H
#define PRACTICA2_TURING_TRANSITION_H

#include <string>
#include <tuple>
#include <vector>

namespace turing {

/**
 * @brief Clave utilizada para identificar una transición de forma determinista.
 */
struct TransitionKey {
    /** @brief Estado origen de la transición. */
    std::string state;

    /** @brief Símbolos leídos por los cabezales, en orden de cinta. */
    std::vector<char> readSymbols;

    /** @brief Ordena las claves para poder almacenarlas en un mapa ordenado. */
    bool operator<(const TransitionKey& other) const;
};

/**
 * @brief Actualización simultánea indicada por una transición.
 *
 * Los vectores tienen un elemento por cinta. Los símbolos de movimiento están
 * restringidos a 'L' y 'R', los movimientos estacionarios no son compatibles
 * intencionadamente.
 */
struct Transition {
    /** @brief Estado al que se entra después de aplicar esta transición. */
    std::string nextState;

    /** @brief Símbolos escritos en las cintas, en orden de cinta. */
    std::vector<char> writeSymbols;

    /** @brief Un movimiento `L` o `R` por cada cinta, en orden de cinta. */
    std::vector<char> movements;
};

}

#endif