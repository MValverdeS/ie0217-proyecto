#ifndef UTILIDADES_HPP
#define UTILIDADES_HPP

#include <string>

#include <sqlite3.h>

double convertirMoneda(double monto, const std::string& monedaOrigen, const std::string& monedaDestino);
void registrarTransaccion(sqlite3* db, int idCuenta, const char* tipoTransaccion, double monto, int idCuentaDestino = -1);
std::string mayusculaLetra1(const std::string& str);

#endif