/**
 * @file Transition.cpp
 * @brief Implementación de la operación de ordenación para las claves de transición.
 * Proyecto: Complejidad Computacional - Práctica 2
 * Curso: 2026/27
 * @author Daniel Palenzuela Álvarez alu0101140469
 */
 
#include "turing/Transition.h"

#include <tuple>

namespace turing {

/** @brief Ordena las claves para poder almacenarlas en un mapa ordenado. */
bool TransitionKey::operator<(const TransitionKey& other) const {
    return std::tie(state, readSymbols) < std::tie(other.state, other.readSymbols);
}

}
