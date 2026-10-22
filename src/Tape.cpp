/**
 * @file Tape.cpp
 * @brief Implementación de la clase Tape, que abstrae una cinta infinita en ambas direcciones con un cabezal de lectura/escritura.
 * Proyecto: Complejidad Computacional - Práctica 2
 * Curso: 2026/27
 * @author Daniel Palenzuela Álvarez alu0101140469
 */

#include "turing/Tape.h"

#include <limits>
#include <sstream>
#include <stdexcept>

namespace turing {

/** @brief Crea una cinta vacía cuyo cabezal se encuentra en la celda cero. */
Tape::Tape(char blankSymbol) : blankSymbol_(blankSymbol) {}

/** @brief Devuelve el símbolo en la posición del cabezal. */
char Tape::read() const { return symbolAt(headPosition_); }

/** @brief Devuelve el símbolo almacenado en una posición arbitraria. */
char Tape::symbolAt(long long position) const {
    const auto it = cells_.find(position);
    return it == cells_.end() ? blankSymbol_ : it->second;
}

/** @brief Escribe un símbolo en la posición del cabezal. Las celdas en blanco no se almacenan. */
void Tape::write(char symbol) {
    if (symbol == blankSymbol_) {
        cells_.erase(headPosition_);
    } else {
        cells_[headPosition_] = symbol;
    }
}

/** @brief Mueve el cabezal una celda a la izquierda o a la derecha. */
void Tape::move(char direction) {
    if (direction == 'L') {
        if (headPosition_ == std::numeric_limits<long long>::min()) {
            throw std::overflow_error("La cabeza ha alcanzado el limite representable de posiciones.");
        }
        --headPosition_;
    } else if (direction == 'R') {
        if (headPosition_ == std::numeric_limits<long long>::max()) {
            throw std::overflow_error("La cabeza ha alcanzado el limite representable de posiciones.");
        }
        ++headPosition_;
    } else {
        throw std::invalid_argument("Movimiento no valido, solo se admiten L y R.");
    }
}

/** @brief Devuelve la coordenada con signo del cabezal en la cinta. */
long long Tape::headPosition() const { return headPosition_; }

/** @brief Devuelve una representación compacta y encierra entre corchetes el símbolo bajo el cabezal. */
std::string Tape::render() const {
    long long first = headPosition_;
    long long last = headPosition_;
    if (!cells_.empty()) {
        if (cells_.begin()->first < first) {
            first = cells_.begin()->first;
        }
        if (cells_.rbegin()->first > last) {
            last = cells_.rbegin()->first;
        }
    }

    // Limita la cantidad de celdas mostradas para evitar salidas excesivamente largas.
    constexpr long long maxDisplayCells = 2000;
    if (last - first > maxDisplayCells) {
        std::ostringstream output;
        output << "[cinta demasiado extensa para mostrar completa, cabeza=" << headPosition_ << "]";
        return output.str();
    }

    // Construye la representación de la cinta, incluyendo el símbolo bajo el cabezal entre corchetes.
    std::string result;
    for (long long position = first; position <= last; ++position) {
        const char symbol = symbolAt(position);
        if (position == headPosition_) {
            result.push_back('[');
            result.push_back(symbol);
            result.push_back(']');
        } else {
            result.push_back(symbol);
        }
        if (position == std::numeric_limits<long long>::max()) {
            break;
        }
    }
    return result;
}

}
