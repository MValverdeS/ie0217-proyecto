/**
 * @file  prestamos.hpp
 * @brief Se decalran las funciones asociadas con prestamos
 *
 * @author Gabriel González Rivera B93432
 * @author Edgar Marcelo Valverde Solís C08089
 * @author Daniel Rodríguez Rivas B96719
 * @date 30/6/2024
 * 
 * Licencia: MIT
 */




#ifndef PRESTAMOS_HPP
#define PRESTAMOS_HPP

#include <sqlite3.h>

void solicitarPrestamo(sqlite3* db);
void verInformacionPrestamos(sqlite3* db);
void calcularTablaPagos(sqlite3* db);
void pagarPrestamo(sqlite3* db);
void verPrestamos(sqlite3* db);
void generarReportePrestamos(sqlite3* db);
void agregarTipoPrestamo(sqlite3* db);
void imprimirTiposPrestamo(sqlite3* db);

#endif