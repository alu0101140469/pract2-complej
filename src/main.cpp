/**
 * @file main.cpp
 * @brief Punto de entrada de la aplicación de consola para el simulador de máquinas de Turing deterministas multicinta.
 * Proyecto: Complejidad Computacional - Práctica 2
 * Curso: 2026/27
 * @author Daniel Palenzuela Álvarez alu0101140469
 */

#include "turing/ConfigurationParser.h"
#include "turing/TuringMachine.h"

#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

// Elimina los espacios en blanco situados al principio y al final de una cadena.
std::string trim(const std::string& value) {
    // Conjunto de caracteres que se consideran espacios en blanco.
    const std::string whitespace = " \t\r\n";
    // Busca la primera posición que no contenga un espacio.
    const std::size_t first = value.find_first_not_of(whitespace);
    // Si toda la cadena son espacios, devuelve una cadena vacía.
    if (first == std::string::npos) {
        return {};
    }
    // Busca el último carácter que no sea un espacio y devuelve el intervalo útil.
    const std::size_t last = value.find_last_not_of(whitespace);
    return value.substr(first, last - first + 1);
}

// Lee las cadenas de entrada desde un fichero, ignorando comentarios y líneas vacías.
std::vector<std::string> loadInputs(const std::string& path) {
    // Intenta abrir el fichero indicado por el usuario.
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("No se puede abrir el fichero de entradas: " + path);
    }

    // Vector que almacenará las cadenas válidas encontradas.
    std::vector<std::string> inputs;
    std::string line;
    std::size_t lineNumber = 0;
    // Procesa el fichero línea a línea.
    while (std::getline(file, line)) {
        ++lineNumber;
        // Normaliza la línea antes de validarla.
        line = trim(line);
        // Las líneas vacías y las que comienzan por '#' son comentarios.
        if (line.empty() || line.front() == '#') {
            continue;
        }
        // La representación "" permite especificar explícitamente epsilon.
        if (line == "\"\"") {
            inputs.emplace_back();
        } else {
            // Cada entrada debe ocupar una sola línea y no contener espacios.
            if (line.find_first_of(" \t") != std::string::npos) {
                throw std::invalid_argument("La entrada de la linea " + std::to_string(lineNumber) +
                                            " contiene espacios, cada cadena debe ocupar una linea.");
            }
            // Añade la entrada al resultado.
            inputs.push_back(line);
        }
    }
    return inputs;
}

// Convierte el argumento de --max-steps a un entero positivo.
std::uint64_t parseMaxSteps(const std::string& value) {
    // Indica cuántos caracteres se han podido convertir correctamente.
    std::size_t consumed = 0;
    unsigned long long parsed = 0;
    try {
        // Convierte la cadena a un entero sin signo.
        parsed = std::stoull(value, &consumed);
    } catch (const std::exception&) {
        // Informa de cualquier valor que no tenga formato numérico válido.
        throw std::invalid_argument("--max-steps requiere un entero positivo.");
    }
    // Rechaza conversiones parciales y el valor cero.
    if (consumed != value.size() || parsed == 0) {
        throw std::invalid_argument("--max-steps requiere un entero positivo.");
    }
    // Devuelve el límite usando el tipo entero empleado por la máquina.
    return static_cast<std::uint64_t>(parsed);
}

// Muestra en pantalla la sintaxis y las opciones disponibles del programa.
void printUsage(const char* program) {
    std::cout << "Uso: " << program << " -config <fichero> [-in <fichero>] [--max-steps <n>]\n"
              << "  -config <fichero>  Definicion de la MT\n"
              << "  -in <fichero>      Cadenas de entrada, una por linea (Opcional).\n"
              << "  --max-steps <n>    Limite de seguridad opcional, no clasifica la ejecucion como parada.\n";
}

// Imprime el resultado de una ejecución y el contenido final de todas las cintas.
void printResult(const std::string& input, const turing::TuringMachine& machine,
                 const turing::RunResult& result) {
    std::cout << "\n============================================================\n";
    std::cout << "Entrada: " << (input.empty() ? "epsilon (cadena vacia)" : input) << '\n';
    std::cout << "Estado final/actual: " << result.finalState << '\n';
    std::cout << "Transiciones ejecutadas: " << result.steps << '\n';

    // Un límite alcanzado no equivale a que la máquina se haya detenido.
    if (result.stepLimitReached) {
        std::cout << "Estado: EJECUCION INTERRUMPIDA por --max-steps; no se considera una parada de la MT.\n";
    } else {
        // Si la ejecución terminó, muestra si la cadena fue aceptada o rechazada.
        std::cout << "Resultado: " << (result.accepted ? "ACEPTADA" : "RECHAZADA") << '\n';
    }

    // Obtiene las cintas para mostrar su contenido y posición de cabeza.
    const auto& tapes = machine.tapes();
    // Recorre todas las cintas de la máquina.
    for (std::size_t index = 0; index < tapes.size(); ++index) {
        std::cout << "Cinta " << (index + 1) << ": " << tapes[index].render()
                  << "  (cabeza=" << tapes[index].headPosition() << ")\n";
    }
}

}

// Punto de entrada del programa.
int main(int argc, char* argv[]) {
    // Rutas de los ficheros y límite opcional de pasos recibidos por argumentos.
    std::string configPath;
    std::string inputPath;
    std::optional<std::uint64_t> maxSteps;

    try {
        // Analiza todos los argumentos de la línea de comandos.
        for (int index = 1; index < argc; ++index) {
            const std::string argument = argv[index];
            // Guarda la ruta del fichero de configuración.
            if (argument == "-config" && index + 1 < argc) {
                configPath = argv[++index];
            // Guarda la ruta opcional del fichero de entradas.
            } else if (argument == "-in" && index + 1 < argc) {
                inputPath = argv[++index];
            // Convierte y guarda el límite opcional de transiciones.
            } else if (argument == "--max-steps" && index + 1 < argc) {
                maxSteps = parseMaxSteps(argv[++index]);
            // Muestra la ayuda y termina correctamente.
            } else if (argument == "-h" || argument == "--help") {
                printUsage(argv[0]);
                return 0;
            // Cualquier otra opción se considera inválida.
            } else {
                printUsage(argv[0]);
                throw std::invalid_argument("Opcion desconocida o argumento ausente: " + argument);
            }
        }
        // El fichero de configuración es obligatorio.
        if (configPath.empty()) {
            printUsage(argv[0]);
            throw std::invalid_argument("Debe indicarse -config <fichero>.");
        }

        // Lee y analiza la definición de la máquina de Turing.
        const turing::ConfigurationParser parser;
        turing::MachineDefinition definition = parser.parseFile(configPath);
        // Crea la máquina a partir de la definición cargada.
        turing::TuringMachine machine(definition);
        // Informa de las características principales de la máquina.
        std::cout << "MT cargada correctamente: " << configPath << '\n'
                  << "Numero de cintas: " << definition.tapeCount() << '\n'
                  << "Alfabeto de entrada Sigma: ";
        // Muestra todos los símbolos del alfabeto de entrada.
        for (const char symbol : definition.inputAlphabet()) {
            std::cout << symbol << ' ';
        }
        // Muestra el símbolo usado como blanco en las cintas.
        std::cout << "\nSimbolo blanco: '" << definition.blankSymbol() << "'\n";

        // Si se proporciona un fichero, procesa automáticamente sus entradas.
        if (!inputPath.empty()) {
            // Carga y filtra las cadenas del fichero.
            const std::vector<std::string> inputs = loadInputs(inputPath);
            if (inputs.empty()) {
                std::cerr << "Aviso: el fichero no contiene cadenas de entrada.\n";
            }
            // Ejecuta la máquina una vez por cada entrada.
            for (const std::string& input : inputs) {
                // Comprueba que la entrada solo usa símbolos permitidos.
                definition.validateInput(input);
                // Inicializa las cintas y la cabeza para la nueva ejecución.
                machine.reset(input);
                // Ejecuta la máquina, posiblemente con un límite de pasos.
                const turing::RunResult result = machine.run(maxSteps);
                // Presenta el resultado al usuario.
                printResult(input, machine, result);
            }
        } else {
            // Sin fichero, activa el modo interactivo por consola.
            std::cout << "Introduce una cadena por linea, una linea vacia representa epsilon y :q termina.\n";
            std::string input;
            // Lee entradas hasta recibir :q o hasta finalizar la entrada estándar.
            while (std::cout << "> " && std::getline(std::cin, input)) {
                // La orden :q finaliza el modo interactivo.
                if (input == ":q") {
                    break;
                }
                try {
                    // Valida los símbolos de la entrada introducida.
                    definition.validateInput(input);
                    // Reinicia la máquina antes de procesar la nueva cadena.
                    machine.reset(input);
                    // Ejecuta la máquina y guarda el resultado obtenido.
                    const turing::RunResult result = machine.run(maxSteps);
                    // Muestra el resultado de la ejecución.
                    printResult(input, machine, result);
                } catch (const std::exception& error) {
                    // Un error de una entrada no finaliza el modo interactivo.
                    std::cerr << "Entrada no valida: " << error.what() << '\n';
                }
            }
        }
    } catch (const std::exception& error) {
        // Captura errores de configuración, argumentos o lectura de ficheros.
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
