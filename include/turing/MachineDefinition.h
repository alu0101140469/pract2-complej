/**
 * @file MachineDefinition.h
 * @brief La definición de la clase MachineDefinition, que almacena y valida la definición formal de una máquina de Turing determinista multi-cinta.
 * Proyecto: Complejidad Computacional - Práctica 2
 * Curso: 2026/27
 * @author Daniel Palenzuela Álvarez alu0101140469
 */
 
#ifndef PRACTICA2_TURING_MACHINE_DEFINITION_H
#define PRACTICA2_TURING_MACHINE_DEFINITION_H

#include "turing/Transition.h"

#include <map>
#include <set>
#include <string>
#include <vector>

namespace turing {

/**
 * @brief Almacena y valida la definición formal de una máquina de Turing.
 *
 * La definición es M = (Q, Sigma, Gamma, s, b, F, delta), ampliada con un
 * número positivo de cintas. Cada transición lee, escribe y mueve todas las
 * cintas simultáneamente.
 */
class MachineDefinition {
public:
    /** @brief Construye una definición vacía, que será rellenada por el analizador. */
    MachineDefinition() = default;

    /** @brief Devuelve el conjunto de estados de la máquina. */
    const std::set<std::string>& states() const;

    /** @brief Devuelve el alfabeto de entrada (que no debe contener el blanco). */
    const std::set<char>& inputAlphabet() const;

    /** @brief Devuelve el alfabeto de cinta. */
    const std::set<char>& tapeAlphabet() const;

    /** @brief Devuelve el estado inicial. */
    const std::string& initialState() const;

    /** @brief Devuelve el símbolo blanco. */
    char blankSymbol() const;

    /** @brief Devuelve el conjunto de estados de aceptación. */
    const std::set<std::string>& acceptingStates() const;

    /** @brief Devuelve el número de cintas. */
    std::size_t tapeCount() const;

    /** @brief Comprueba si un estado pertenece al conjunto de aceptación. */
    bool isAccepting(const std::string& state) const;

    /** @brief Busca una transición para un estado y símbolos de lectura dados. */
    const Transition* findTransition(const std::string& state,
                                     const std::vector<char>& readSymbols) const;

    /** @brief Rechaza cadenas de entrada que contienen símbolos fuera de Sigma. */
    void validateInput(const std::string& input) const;

    /** @brief Añade un estado a Q. Los estados duplicados son rechazados por la validación. */
    void addState(const std::string& state);

    /** @brief Añade un símbolo al alfabeto de entrada. */
    void addInputSymbol(char symbol);

    /** @brief Añade un símbolo al alfabeto de cinta. */
    void addTapeSymbol(char symbol);

    /** @brief Establece el estado inicial. */
    void setInitialState(const std::string& state);

    /** @brief Establece el símbolo blanco. */
    void setBlankSymbol(char symbol);

    /** @brief Añade un estado de aceptación. */
    void addAcceptingState(const std::string& state);

    /** @brief Establece el número de cintas. */
    void setTapeCount(std::size_t count);

    /** @brief Añade una transición determinista y rechaza claves duplicadas. */
    void addTransition(const TransitionKey& key, const Transition& transition);

    /** @brief Comprueba las restricciones formales de la definición completa. */
    void validate() const;

private:
// Atributos de la definición formal de la máquina de Turing
    std::set<std::string> states_;
    std::set<char> inputAlphabet_;
    std::set<char> tapeAlphabet_;
    std::string initialState_;
    char blankSymbol_ = '.';
    std::set<std::string> acceptingStates_;
    std::size_t tapeCount_ = 0;
    std::map<TransitionKey, Transition> transitions_;
};

}
#endif
