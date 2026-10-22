/**
 * @file test_turing_machine.cpp
 * @brief Pruebas unitarias de la clase TuringMachine y de la definición de máquinas de Turing.
 * Proyecto: Complejidad Computacional - Práctica 2
 * Curso: 2026/27
 * @author Daniel Palenzuela Álvarez alu0101140469
 */

#include "turing/ConfigurationParser.h"
#include "turing/TuringMachine.h"

#include <cstdlib>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void require(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

// Devuelve la representación de la cinta sin los corchetes que indican la posición del cabezal.
std::string tapeWithoutHeadBrackets(const turing::Tape& tape) {
    std::string rendered = tape.render();
    const std::string::size_type left = rendered.find('[');
    if (left != std::string::npos) {
        rendered.erase(left, 1);
        const std::string::size_type right = rendered.find(']', left);
        if (right != std::string::npos) {
            rendered.erase(right, 1);
        }
    }
    return rendered;
}

// Prueba de la máquina que reconoce el lenguaje a^n b^m con m >= n > 0.
void testLanguageMachine() {
    const turing::ConfigurationParser parser;
    const auto definition = parser.parseFile("machines/MT1_lenguaje.txt");
    turing::TuringMachine machine(definition);

    const auto accepts = [&machine](const std::string& input) {
        machine.reset(input);
        const auto result = machine.run(100000);
        require(result.halted && !result.stepLimitReached, "MT1 no se detuvo para la entrada: " + input);
        return result.accepted;
    };

    require(accepts("ab"), "MT1 debe aceptar ab.");
    require(accepts("aabbb"), "MT1 debe aceptar aabbb.");
    require(accepts("aaabbb"), "MT1 debe aceptar aaabbb.");
    require(!accepts("aab"), "MT1 debe rechazar aab: hay menos b que a.");
    require(!accepts("b"), "MT1 debe rechazar b: n debe ser positivo.");
    require(!accepts("aba"), "MT1 debe rechazar una entrada que no sea a^n b^m.");
    require(!accepts(""), "MT1 debe rechazar la cadena vacia.");

    // Prueba exhaustiva de todas las cadenas de longitud hasta 7 sobre el alfabeto {a, b}.
    for (std::size_t length = 0; length <= 7; ++length) {
        const std::size_t combinations = static_cast<std::size_t>(1U) << length;
        for (std::size_t mask = 0; mask < combinations; ++mask) {
            std::string input;
            for (std::size_t index = 0; index < length; ++index) {
                input.push_back((mask & (static_cast<std::size_t>(1U) << index)) != 0U ? 'b' : 'a');
            }
            std::size_t aCount = 0;
            while (aCount < input.size() && input[aCount] == 'a') {
                ++aCount;
            }
            const bool suffixHasOnlyB = input.find('a', aCount) == std::string::npos;
            const std::size_t bCount = input.size() - aCount;
            const bool expected = aCount > 0 && suffixHasOnlyB && bCount >= aCount;
            require(accepts(input) == expected, "MT1 fallo en la entrada exhaustiva: " + input);
        }
    }
}

// Prueba de la máquina que cuenta símbolos a y b y los representa en formato binario.
void testCounterMachine() {
    const turing::ConfigurationParser parser;
    const auto definition = parser.parseFile("machines/MT2_conteo.txt");
    turing::TuringMachine machine(definition);

    const auto outputFor = [&machine](const std::string& input) {
        machine.reset(input);
        const auto result = machine.run(1000000);
        require(result.halted && !result.stepLimitReached, "MT2 no se detuvo para la entrada: " + input);
        require(result.accepted, "MT2 debe aceptar y transformar toda cadena de Sigma: " + input);
        require(!machine.tapes().front().render().empty() &&
                    machine.tapes().front().render().front() == '[',
                "La cabeza de la cinta 1 debe quedar al principio del resultado.");
        return tapeWithoutHeadBrackets(machine.tapes().front());
    };

    require(outputFor("abbabaabb") == "11111.1111", "MT2 debe transformar abbabaabb en 11111.1111.");
    require(outputFor("aa") == "0.11", "MT2 debe representar cero b como 0.");
    require(outputFor("bb") == "11.0", "MT2 debe representar cero a como 0.");
    require(outputFor("a") == "0.1", "MT2 debe transformar a en 0.1.");
    require(outputFor("b") == "1.0", "MT2 debe transformar b en 1.0.");
    require(outputFor("") == "0.0", "MT2 debe transformar la cadena vacia en 0.0.");

    for (std::size_t length = 0; length <= 7; ++length) {
        const std::size_t combinations = static_cast<std::size_t>(1U) << length;
        for (std::size_t mask = 0; mask < combinations; ++mask) {
            std::string input;
            for (std::size_t index = 0; index < length; ++index) {
                input.push_back((mask & (static_cast<std::size_t>(1U) << index)) != 0U ? 'b' : 'a');
            }
            std::size_t aCount = 0;
            std::size_t bCount = 0;
            for (const char symbol : input) {
                if (symbol == 'a') {
                    ++aCount;
                } else {
                    ++bCount;
                }
            }
            const std::string expected = (bCount == 0 ? "0" : std::string(bCount, '1')) + "." +
                                         (aCount == 0 ? "0" : std::string(aCount, '1'));
            require(outputFor(input) == expected, "MT2 fallo en la entrada exhaustiva: " + input);
            require(machine.tapes().front().headPosition() < 0,
                    "La cabeza de MT2 debe quedar al comienzo del resultado.");
        }
    }
}

// Prueba de la máquina multicinta que copia la primera cinta a la segunda.
void testMultiTapeDemo() {
    const turing::ConfigurationParser parser;
    const auto definition = parser.parseFile("machines/examples/MT_multicinta_demo.txt");
    require(definition.tapeCount() == 2, "La maquina demostrativa debe tener dos cintas.");
    turing::TuringMachine machine(definition);
    machine.reset("abba");
    const auto result = machine.run(100);
    require(result.halted && result.accepted, "La maquina multicinta debe aceptar abba.");
    require(tapeWithoutHeadBrackets(machine.tapes()[0]) == "abba", "La cinta 1 debe conservar abba.");
    require(tapeWithoutHeadBrackets(machine.tapes()[1]) == "abba", "La cinta 2 debe recibir una copia de abba.");
}

// Prueba de los ejemplos proporcionados en el enunciado.
void testProvidedExamples() {
    const turing::ConfigurationParser parser;
    const auto oddZeroDefinition = parser.parseFile("machines/examples/Ejemplo1_MT.txt");
    turing::TuringMachine oddZeroMachine(oddZeroDefinition);
    oddZeroMachine.reset("0");
    require(oddZeroMachine.run(100).accepted, "Ejemplo1 debe aceptar una cadena con un cero.");
    oddZeroMachine.reset("00");
    require(!oddZeroMachine.run(100).accepted, "Ejemplo1 debe rechazar una cadena con dos ceros.");

    const auto doubleUnaryDefinition = parser.parseFile("machines/examples/Ejemplo2_MT.txt");
    turing::TuringMachine doubleUnaryMachine(doubleUnaryDefinition);
    doubleUnaryMachine.reset("111");
    const auto result = doubleUnaryMachine.run(1000);
    require(result.halted && result.accepted, "Ejemplo2 debe detenerse y aceptar 111.");
    require(tapeWithoutHeadBrackets(doubleUnaryMachine.tapes().front()) == "111111",
            "Ejemplo2 debe duplicar 111 a 111111.");
}

// Prueba de que el símbolo reservado para blanco no puede usarse como entrada.
void testReservedBlank() {
    const turing::ConfigurationParser parser;
    const auto definition = parser.parseFile("machines/MT1_lenguaje.txt");
    bool rejected = false;
    try {
        definition.validateInput("a.b");
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    require(rejected, "El punto debe rechazarse como simbolo de entrada.");
}

}

// Entrada principal de las pruebas unitarias.
int main() {
    try {
        testLanguageMachine();
        testCounterMachine();
        testMultiTapeDemo();
        testProvidedExamples();
        testReservedBlank();
        std::cout << "Todas las pruebas C++ han pasado.\n";
    } catch (const std::exception& error) {
        std::cerr << "FALLO: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
