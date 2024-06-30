/**
 * @file  cdp.cpp
 * @brief Se declaran las funciones correspondientes a cdp
 *
 * @author Gabriel González Rivera B93432
 * @author Edgar Marcelo Valverde Solís C08089
 * @author Daniel Rodríguez Rivas B96719
 * @date 30/6/2024
 * 
 * Licencia: MIT
 */



#ifndef CDP_HPP
#define CDP_HPP

#include <sqlite3.h>

void crearCDP(sqlite3* db);
void verCDP(sqlite3* db);
void eliminarCDP(sqlite3* db);

#endif