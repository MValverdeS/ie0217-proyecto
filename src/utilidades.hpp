
/**
 * @file  prestamos.cpp
 * @brief Se declaran las funciones asociadas con prestamos
 *
 * @author Gabriel González Rivera B93432
 * @author Edgar Marcelo Valverde Solís C08089
 * @author Daniel Rodríguez Rivas B96719
 * @date 30/6/2024
 * 
 * Licencia: MIT
 */



#ifndef UTILIDADES_HPP
#define UTILIDADES_HPP

#include <string>

#include <sqlite3.h>

double convertirMoneda(double monto, const std::string& monedaOrigen, const std::string& monedaDestino);
void registrarTransaccion(sqlite3* db, int idCuenta, const char* tipoTransaccion, double monto, int idCuentaDestino = -1);
std::string mayusculaLetra1(const std::string& str);

#endif