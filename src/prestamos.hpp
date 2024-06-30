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