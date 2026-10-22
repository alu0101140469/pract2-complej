/**
 * @file Tape.h
 * @brief Abstracción de una cinta infinita en ambas direcciones con un cabezal de lectura/escritura.
 * Proyecto: Complejidad Computacional - Práctica 2
 * Curso: 2026/27
 * @author Daniel Palenzuela Álvarez alu0101140469
 */
 
#ifndef PRACTICA2_TURING_TAPE_H
#define PRACTICA2_TURING_TAPE_H

#include <map>
#include <string>

namespace turing {

/** @brief Representa una cinta infinita en ambas direcciones. */
class Tape {
public:
    /** @brief Crea una cinta vacía cuyo cabezal se encuentra en la celda cero. */
    explicit Tape(char blankSymbol = '.');

    /** @brief Devuelve el símbolo en la posición del cabezal. */
    char read() const;

    /** @brief Devuelve el símbolo almacenado en una posición arbitraria. */
    char symbolAt(long long position) const;

    /** @brief Escribe un símbolo en la posición del cabezal. Las celdas en blanco no se almacenan. */
    void write(char symbol);

    /** @brief Mueve el cabezal una celda a la izquierda o a la derecha. */
    void move(char direction);

    /** @brief Devuelve la coordenada con signo del cabezal en la cinta. */
    long long headPosition() const;

    /** @brief Devuelve una representación compacta y encierra entre corchetes el símbolo bajo el cabezal. */
    std::string render() const;

private:
    char blankSymbol_;
    long long headPosition_ = 0;
    std::map<long long, char> cells_;
};

}

#endif