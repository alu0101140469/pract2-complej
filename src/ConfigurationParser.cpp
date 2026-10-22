/**
 * @file ConfigurationParser.cpp
 * @brief Implementación de la clase ConfigurationParser, que se encarga de parsear y validar el formato de configuración usado por el simulador.
 * Proyecto: Complejidad Computacional - Práctica 2
 * Curso: 2026/27
 * @author Daniel Palenzuela Álvarez alu0101140469
 */

#include "turing/ConfigurationParser.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace turing {
namespace {

    /** @brief Elimina espacios en blanco al principio y al final de una cadena. */
std::string trim(const std::string& value) {
    const std::string whitespace = " \t\r\n";
    const std::size_t first = value.find_first_not_of(whitespace);
    if (first == std::string::npos) {
        return {};
    }
    const std::size_t last = value.find_last_not_of(whitespace);
    return value.substr(first, last - first + 1);
}

/** @brief Divide una cadena en tokens separados por espacios. */
std::vector<std::string> split(const std::string& value) {
    std::istringstream stream(value);
    std::vector<std::string> result;
    std::string token;
    while (stream >> token) {
        result.push_back(token);
    }
    return result;
}

/** @brief Obtiene una línea obligatoria de la configuración y la divide en tokens. 
    * @param lines Líneas de la configuración.
    * @param index Índice de la línea a obtener.
    * @param name Nombre de la línea a obtener.
    * @return Tokens de la línea.
    * @throw std::invalid_argument Si la línea no existe o está vacía.
    */
std::vector<std::string> requireLine(const std::vector<std::string>& lines,
                                    std::size_t index,
                                    const std::string& name) {
    if (index >= lines.size()) {
        throw std::invalid_argument("Falta la linea obligatoria: " + name + ".");
    }
    const auto tokens = split(lines[index]);
    if (tokens.empty()) {
        throw std::invalid_argument("La linea obligatoria esta vacia: " + name + ".");
    }
    return tokens;
}

/** @brief Convierte un token en un símbolo de un solo carácter, o lanza una excepción si no es válido. 
    * @param token Token a convertir.
    * @param fieldName Nombre del campo del que proviene el token.
    * @return Símbolo de un solo carácter.
    * @throw std::invalid_argument Si el token no representa un símbolo válido.
    */
char parseSymbol(const std::string& token, const std::string& fieldName) {
    if (token.size() != 1) {
        throw std::invalid_argument("Cada simbolo de " + fieldName + " debe ser un unico caracter: '" + token + "'.");
    }
    return token.front();
}

/** @brief Convierte un token en un entero positivo, o lanza una excepción si no es válido. 
    * @param token Token a convertir.
    * @return Entero positivo.
    * @throw std::invalid_argument Si el token no representa un entero positivo.
    */
std::size_t parsePositiveSize(const std::string& token) {
    std::size_t consumed = 0;
    unsigned long long parsed = 0;
    try {
        parsed = std::stoull(token, &consumed);
    } catch (const std::exception&) {
        throw std::invalid_argument("El numero de cintas debe ser un entero positivo.");
    }
    if (consumed != token.size() || parsed == 0 || parsed > static_cast<unsigned long long>(1000)) {
        throw std::invalid_argument("El numero de cintas debe ser un entero entre 1 y 1000.");
    }
    return static_cast<std::size_t>(parsed);
}

}

/**
 * @brief Parsea un archivo de configuración y devuelve una definición de máquina de Turing.
 * @param path Ruta al archivo de configuración.
 * @return Definición de la máquina de Turing.
 */
MachineDefinition ConfigurationParser::parseFile(const std::string& path) const {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("No se puede abrir el fichero de configuracion: " + path);
    }

    // Lee todas las líneas, eliminando comentarios y líneas vacías.
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        const std::string cleaned = trim(line);
        if (cleaned.empty() || cleaned.front() == '#') {
            continue;
        }
        lines.push_back(cleaned);
    }
    // Comprueba que hay al menos 7 líneas obligatorias: Q, Sigma, Gamma, estado inicial, blanco, F y número de cintas.
    if (lines.size() < 7) {
        throw std::invalid_argument("Configuracion incompleta, serequieren Q, Sigma, Gamma, estado inicial, blanco, F y numero de cintas.");
    }

    // Construye la definición de la máquina a partir de las líneas obligatorias.
    MachineDefinition definition;
    for (const std::string& state : requireLine(lines, 0, "Q")) {
        definition.addState(state);
    }
    for (const std::string& token : requireLine(lines, 1, "Sigma")) {
        definition.addInputSymbol(parseSymbol(token, "Sigma"));
    }
    for (const std::string& token : requireLine(lines, 2, "Gamma")) {
        definition.addTapeSymbol(parseSymbol(token, "Gamma"));
    }
    
    // Procesa las líneas de estado inicial, símbolo blanco, estados de aceptación y número de cintas.
    const auto initialTokens = requireLine(lines, 3, "estado inicial");
    if (initialTokens.size() != 1) {
        throw std::invalid_argument("La linea del estado inicial debe contener un unico estado.");
    }
    definition.setInitialState(initialTokens.front());

    // Procesa la línea del símbolo blanco.
    const auto blankTokens = requireLine(lines, 4, "simbolo blanco");
    if (blankTokens.size() != 1) {
        throw std::invalid_argument("La linea del simbolo blanco debe contener un unico caracter.");
    }
    definition.setBlankSymbol(parseSymbol(blankTokens.front(), "simbolo blanco"));

    // Procesa la línea de estados de aceptación.
    const auto finalTokens = requireLine(lines, 5, "F");
    if (!(finalTokens.size() == 1 && finalTokens.front() == "-")) {
        for (const std::string& state : finalTokens) {
            definition.addAcceptingState(state);
        }
    }

    // Procesa la línea del número de cintas y las transiciones.
    const auto tapeTokens = requireLine(lines, 6, "numero de cintas");
    if (tapeTokens.size() != 1) {
        throw std::invalid_argument("La linea del numero de cintas debe contener un unico entero.");
    }
    const std::size_t tapeCount = parsePositiveSize(tapeTokens.front());
    definition.setTapeCount(tapeCount);

    // Procesa las líneas de transición
    const std::size_t expectedTokens = 2 + (3 * tapeCount);
    for (std::size_t lineIndex = 7; lineIndex < lines.size(); ++lineIndex) {
        const std::vector<std::string> tokens = split(lines[lineIndex]);
        if (tokens.size() != expectedTokens) {
            std::ostringstream message;
            message << "Transicion mal formada en la linea de datos " << (lineIndex + 1)
                    << ": se esperaban " << expectedTokens << " elementos para " << tapeCount
                    << " cinta(s), pero se encontraron " << tokens.size() << ".";
            throw std::invalid_argument(message.str());
        }

        TransitionKey key;
        key.state = tokens[0];
        for (std::size_t tape = 0; tape < tapeCount; ++tape) {
            key.readSymbols.push_back(parseSymbol(tokens[1 + tape], "lectura de transicion"));
        }

        Transition transition;
        transition.nextState = tokens[1 + tapeCount];
        for (std::size_t tape = 0; tape < tapeCount; ++tape) {
            transition.writeSymbols.push_back(parseSymbol(tokens[2 + tapeCount + tape], "escritura de transicion"));
        }
        for (std::size_t tape = 0; tape < tapeCount; ++tape) {
            const std::string& movement = tokens[2 + (2 * tapeCount) + tape];
            if (movement.size() != 1) {
                throw std::invalid_argument("Cada movimiento debe ser L o R.");
            }
            transition.movements.push_back(movement.front());
        }
        definition.addTransition(key, transition);
    }

    definition.validate();
    return definition;
}

}
