/**
 * @file cuentas.hpp
 * @brief Se declaran las funciones asociadas con cuentas
 *
 * @author Gabriel González Rivera B93432
 * @author Edgar Marcelo Valverde Solís C08089
 * @author Daniel Rodríguez Rivas B96719
 * @date 30/6/2024
 * 
 * Licencia: MIT
 */


#ifndef CUENTAS_HPP
#define CUENTAS_HPP

#include <sqlite3.h>

void crearCuenta(sqlite3* db);
void verCuentas(sqlite3* db);
void realizarDeposito(sqlite3* db);
void realizarRetiro(sqlite3* db);
void realizarTransferencia(sqlite3* db);
void verTransacciones(sqlite3* db);

#endif