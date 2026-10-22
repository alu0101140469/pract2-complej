/**
 * @file ConfigurationParser.h
 * @brief La definición de la clase ConfigurationParser, que se encarga de parsear y validar el formato de configuración usado por el simulador.
 * Proyecto: Complejidad Computacional - Práctica 2
 * Curso: 2026/27
 * @author Daniel Palenzuela Álvarez alu0101140469
 */
 
#ifndef PRACTICA2_TURING_CONFIGURATION_PARSER_H
#define PRACTICA2_TURING_CONFIGURATION_PARSER_H

#include "turing/MachineDefinition.h"

#include <string>

namespace turing {

/** @brief Clase que se encarga de parsear y validar el formato de configuración usado por el simulador. */
class ConfigurationParser {
public:
    /** @brief Parsea un fichero de configuración y devuelve la definición de la máquina de Turing correspondiente.
     *  @param path La ruta del fichero de configuración.
     *  @return La definición de la máquina de Turing.
     *  @throws std::runtime_error Si no se puede abrir el fichero.
     *  @throws std::invalid_argument Si el fichero contiene errores de formato o semántica.
     */
    MachineDefinition parseFile(const std::string& path) const;
};

}

#endif
