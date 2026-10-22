
 /**
 * @file MachineDefinition.cpp
 * @brief Implementación de la clase ConfigurationParser, que se encarga de parsear y validar el formato de configuración usado por el simulador.
 * Proyecto: Complejidad Computacional - Práctica 2
 * Curso: 2026/27
 * @author Daniel Palenzuela Álvarez alu0101140469
 */

#include "turing/MachineDefinition.h"

#include <sstream>
#include <stdexcept>

namespace turing {

/** @brief Devuelve el conjunto de estados de la maquina. */
const std::set<std::string>& MachineDefinition::states() const { return states_; }
/** @brief Devuelve el alfabeto de entrada, que no puede contener el blanco. */
const std::set<char>& MachineDefinition::inputAlphabet() const { return inputAlphabet_; }
/** @brief Devuelve el alfabeto completo de la cinta. */
const std::set<char>& MachineDefinition::tapeAlphabet() const { return tapeAlphabet_; }
/** @brief Devuelve el estado inicial de la maquina. */
const std::string& MachineDefinition::initialState() const { return initialState_; }
/** @brief Devuelve el simbolo utilizado como blanco. */
char MachineDefinition::blankSymbol() const { return blankSymbol_; }
/** @brief Devuelve el conjunto de estados de aceptacion. */
const std::set<std::string>& MachineDefinition::acceptingStates() const { return acceptingStates_; }
/** @brief Devuelve el numero de cintas de la maquina. */
std::size_t MachineDefinition::tapeCount() const { return tapeCount_; }

/** @brief Indica si un estado pertenece al conjunto de aceptacion. */
bool MachineDefinition::isAccepting(const std::string& state) const {
    // La busqueda evita modificar el conjunto de estados aceptores.
    return acceptingStates_.find(state) != acceptingStates_.end();
}

/** @brief Busca la transicion asociada a un estado y una lectura concreta. */
const Transition* MachineDefinition::findTransition(
    const std::string& state, const std::vector<char>& readSymbols) const {
    // Se construye la clave con el estado y los simbolos leidos en cada cinta.
    const auto it = transitions_.find(TransitionKey{state, readSymbols});
    // Se devuelve nullptr cuando no existe una transicion aplicable.
    return it == transitions_.end() ? nullptr : &it->second;
}

/** @brief Comprueba que todos los simbolos de una entrada sean validos. */
void MachineDefinition::validateInput(const std::string& input) const {
    // La entrada se revisa simbolo a simbolo para localizar el primer error.
    for (const char symbol : input) {
        // El punto representa el blanco y esta reservado para las cintas.
        if (symbol == '.') {
            throw std::invalid_argument(
                "El simbolo '.' esta reservado para el blanco y no puede aparecer en la entrada.");
        }
        // Todo simbolo de entrada debe pertenecer a Sigma.
        if (inputAlphabet_.find(symbol) == inputAlphabet_.end()) {
            std::ostringstream message;
            message << "El simbolo '" << symbol << "' no pertenece al alfabeto de entrada Sigma.";
            throw std::invalid_argument(message.str());
        }
    }
}

/** @brief Inserta un estado en el conjunto de estados. */
void MachineDefinition::addState(const std::string& state) { states_.insert(state); }
/** @brief Inserta un simbolo en el alfabeto de entrada. */
void MachineDefinition::addInputSymbol(char symbol) { inputAlphabet_.insert(symbol); }
/** @brief Inserta un simbolo en el alfabeto de cinta. */
void MachineDefinition::addTapeSymbol(char symbol) { tapeAlphabet_.insert(symbol); }
/** @brief Establece el estado inicial. */
void MachineDefinition::setInitialState(const std::string& state) { initialState_ = state; }
/** @brief Establece el simbolo blanco de las cintas. */
void MachineDefinition::setBlankSymbol(char symbol) { blankSymbol_ = symbol; }
/** @brief Inserta un estado en el conjunto de aceptacion. */
void MachineDefinition::addAcceptingState(const std::string& state) { acceptingStates_.insert(state); }
/** @brief Establece el numero de cintas. */
void MachineDefinition::setTapeCount(std::size_t count) { tapeCount_ = count; }

/** @brief Anade una transicion y rechaza claves duplicadas. */
void MachineDefinition::addTransition(const TransitionKey& key, const Transition& transition) {
    // emplace devuelve si la insercion se realizo o si ya existia la clave.
    const auto inserted = transitions_.emplace(key, transition);
    if (!inserted.second) {
        throw std::invalid_argument("La definicion no es determinista, transicion duplicada para el mismo estado y lectura.");
    }
}

/** @brief Valida la coherencia completa de la definicion de la maquina. */
void MachineDefinition::validate() const {
    // Una maquina debe tener al menos un estado.
    if (states_.empty()) {
        throw std::invalid_argument("El conjunto de estados Q no puede estar vacio.");
    }
    // El estado inicial debe estar declarado en Q.
    if (initialState_.empty() || states_.find(initialState_) == states_.end()) {
        throw std::invalid_argument("El estado inicial s debe pertenecer a Q.");
    }
    // No se permiten maquinas sin cintas.
    if (tapeCount_ == 0) {
        throw std::invalid_argument("El numero de cintas debe ser mayor que cero.");
    }
    // El blanco debe existir en Gamma.
    if (tapeAlphabet_.empty() || tapeAlphabet_.find(blankSymbol_) == tapeAlphabet_.end()) {
        throw std::invalid_argument("El simbolo blanco debe pertenecer al alfabeto de cinta Gamma.");
    }
    // Sigma y el blanco deben ser disjuntos.
    if (inputAlphabet_.find(blankSymbol_) != inputAlphabet_.end()) {
        throw std::invalid_argument("El simbolo blanco no puede pertenecer al alfabeto de entrada Sigma.");
    }
    // El punto se reserva para representar el blanco.
    if (inputAlphabet_.find('.') != inputAlphabet_.end()) {
        throw std::invalid_argument("El simbolo '.' esta reservado y no puede pertenecer a Sigma.");
    }
    // Cada simbolo de entrada debe estar disponible tambien en la cinta.
    for (const char symbol : inputAlphabet_) {
        if (tapeAlphabet_.find(symbol) == tapeAlphabet_.end()) {
            throw std::invalid_argument("Sigma debe estar contenido en Gamma.");
        }
    }
    // Todos los estados aceptores deben estar declarados en Q.
    for (const std::string& state : acceptingStates_) {
        if (states_.find(state) == states_.end()) {
            throw std::invalid_argument("Todos los estados de aceptacion deben pertenecer a Q.");
        }
    }
    // Se valida individualmente cada transicion almacenada.
    for (const auto& entry : transitions_) {
        const TransitionKey& key = entry.first;
        const Transition& transition = entry.second;
        // Tanto el estado origen como el destino deben existir.
        if (states_.find(key.state) == states_.end() || states_.find(transition.nextState) == states_.end()) {
            throw std::invalid_argument("Cada estado origen y destino de una transicion debe pertenecer a Q.");
        }
        // Cada transicion debe especificar una operacion para cada cinta.
        if (key.readSymbols.size() != tapeCount_ || transition.writeSymbols.size() != tapeCount_ ||
            transition.movements.size() != tapeCount_) {
            throw std::invalid_argument("Cada transicion debe incluir lectura, escritura y movimiento para todas las cintas.");
        }
        // Ningun simbolo leido puede estar fuera de Gamma.
        for (const char symbol : key.readSymbols) {
            if (tapeAlphabet_.find(symbol) == tapeAlphabet_.end()) {
                throw std::invalid_argument("Una transicion lee un simbolo fuera de Gamma.");
            }
        }
        // Ningun simbolo escrito puede estar fuera de Gamma.
        for (const char symbol : transition.writeSymbols) {
            if (tapeAlphabet_.find(symbol) == tapeAlphabet_.end()) {
                throw std::invalid_argument("Una transicion escribe un simbolo fuera de Gamma.");
            }
        }
        // Solo se admiten desplazamientos a izquierda o derecha.
        for (const char movement : transition.movements) {
            if (movement != 'L' && movement != 'R') {
                throw std::invalid_argument("Los unicos movimientos permitidos son L y R.");
            }
        }
    }
}

}
