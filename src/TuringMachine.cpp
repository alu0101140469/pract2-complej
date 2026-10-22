/**
 * @file TuringMachine.cpp
 * @brief Implementación de la clase TuringMachine, que simula una máquina de Turing determinista multicinta.
 * Proyecto: Complejidad Computacional - Práctica 2
 * Curso: 2026/27
 * @author Daniel Palenzuela Álvarez alu0101140469
 */
 
#include "turing/TuringMachine.h"

// Excepciones estándar utilizadas por las clases del proyecto.
#include <stdexcept>
// Permite mover la definición de la máquina sin copiarla.
#include <utility>

namespace turing {

// Construye una máquina a partir de su definición y prepara todas sus cintas.
TuringMachine::TuringMachine(MachineDefinition definition)
    // La definición se mueve al miembro interno y se establece el estado inicial.
    : definition_(std::move(definition)), currentState_(definition_.initialState()) {
    // Comprueba que la definición de la máquina sea coherente.
    definition_.validate();
    // Reserva espacio para el número de cintas indicado por la definición.
    tapes_.reserve(definition_.tapeCount());
    // Inicializa cada cinta con el símbolo blanco.
    for (std::size_t index = 0; index < definition_.tapeCount(); ++index) {
        tapes_.emplace_back(definition_.blankSymbol());
    }
}

// Reinicia la máquina y coloca la entrada en la primera cinta.
void TuringMachine::reset(const std::string& input) {
    // Verifica que la entrada pertenezca al alfabeto permitido.
    definition_.validateInput(input);
    // Elimina el contenido anterior de las cintas.
    tapes_.clear();
    // Reserva de nuevo el espacio necesario para todas las cintas.
    tapes_.reserve(definition_.tapeCount());
    // Crea las cintas vacías, posicionando cada cabezal en su primera celda.
    for (std::size_t index = 0; index < definition_.tapeCount(); ++index) {
        tapes_.emplace_back(definition_.blankSymbol());
    }
    // Escribe la entrada en la primera cinta y avanza entre sus símbolos.
    for (std::size_t index = 0; index < input.size(); ++index) {
        tapes_.front().write(input[index]);
        if (index + 1 < input.size()) {
            tapes_.front().move('R');
        }
    }
    // Si la entrada no está vacía, devuelve el cabezal a la primera celda de la entrada.
    if (!input.empty()) {
        for (std::size_t index = 1; index < input.size(); ++index) {
            tapes_.front().move('L');
        }
    }
    // Restablece el estado inicial y marca la máquina como no detenida.
    currentState_ = definition_.initialState();
    halted_ = false;
}

// Obtiene la transición aplicable al estado y a los símbolos actuales.
const Transition* TuringMachine::currentTransition() const {
    // Recoge el símbolo situado bajo el cabezal de cada cinta.
    std::vector<char> readSymbols;
    readSymbols.reserve(tapes_.size());
    for (const Tape& tape : tapes_) {
        readSymbols.push_back(tape.read());
    }
    // Busca una transición que coincida con el estado y los símbolos leídos.
    return definition_.findTransition(currentState_, readSymbols);
}

// Ejecuta una transición de la máquina y devuelve si se ha realizado.
bool TuringMachine::performStep() {
    // Una máquina detenida no puede ejecutar más pasos.
    if (halted_) {
        return false;
    }
    // Determina la transición correspondiente a la configuración actual.
    const Transition* transition = currentTransition();
    // La ausencia de transición implica que la máquina se detiene.
    if (transition == nullptr) {
        halted_ = true;
        return false;
    }

    // Escribe en cada cinta el símbolo indicado por la transición.
    for (std::size_t index = 0; index < tapes_.size(); ++index) {
        tapes_[index].write(transition->writeSymbols[index]);
    }
    // Mueve cada cabezal en la dirección indicada por la transición.
    for (std::size_t index = 0; index < tapes_.size(); ++index) {
        tapes_[index].move(transition->movements[index]);
    }
    // Actualiza el estado después de aplicar todas las operaciones de las cintas.
    currentState_ = transition->nextState;
    return true;
}

// Ejecuta la máquina hasta que se detenga o alcance el límite de pasos.
RunResult TuringMachine::run(std::optional<std::uint64_t> maxSteps) {
    // Contador de pasos ejecutados durante esta ejecución.
    std::uint64_t steps = 0;
    while (true) {
        // Si no existe una transición aplicable, la máquina se detiene y se devuelve el resultado.
        if (currentTransition() == nullptr) {
            // Marca la máquina como detenida y devuelve si acepta la entrada.
            halted_ = true;
            return RunResult{true, definition_.isAccepting(currentState_), false, steps, currentState_};
        }
        // Si existe un límite y ya se alcanzó, finaliza por agotamiento del límite.
        if (maxSteps.has_value() && steps >= maxSteps.value()) {
            return RunResult{false, false, true, steps, currentState_};
        }
        // Ejecuta una transición y, si tiene éxito, incrementa el contador.
        if (!performStep()) {
            // Este caso cubre una detención detectada durante la ejecución del paso.
            return RunResult{true, definition_.isAccepting(currentState_), false, steps, currentState_};
        }
        ++steps;
    }
}

// Devuelve el estado actual de la máquina.
const std::string& TuringMachine::currentState() const { return currentState_; }
// Devuelve las cintas y el contenido visible de la máquina.
const std::vector<Tape>& TuringMachine::tapes() const { return tapes_; }
// Devuelve la definición inmutable de la máquina.
const MachineDefinition& TuringMachine::definition() const { return definition_; }

}
