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