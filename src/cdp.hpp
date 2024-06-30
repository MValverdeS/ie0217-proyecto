#ifndef CDP_HPP
#define CDP_HPP

#include <sqlite3.h>

void crearCDP(sqlite3* db);
void verCDP(sqlite3* db);
void eliminarCDP(sqlite3* db);

#endif