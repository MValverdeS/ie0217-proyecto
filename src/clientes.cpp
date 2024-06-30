/**
 * @file  clientes.cpp
 * @brief Se definen las funciones asociadas con clientes
 *
 * @author Gabriel González Rivera B93432
 * @author Edgar Marcelo Valverde Solís C08089
 * @author Daniel Rodríguez Rivas B96719
 * @date 30/6/2024
 * 
 * Licencia: MIT
 */




#include <iostream>
#include <sqlite3.h>

#include "clientes.hpp"

#include <cmath>
#include <fstream>

using namespace std;

/**
 * @brief Agrega un nuevo cliente a la base de datos.
 * 
 * Esta función permite al usuario ingresar los detalles de un nuevo cliente y lo agrega a la tabla `INFOCLIENTES` en la base de datos.
 * 
 * @param db Puntero a la base de datos SQLite.
 */
void agregarCliente(sqlite3* db) {
    int idCliente, cedula;
    string nombre, apellido;

    cout << "Ingrese la Cedula del Cliente: ";
    cin >> cedula;

    // Verificar si la cédula ya está registrada
    const char *sqlCheckCedula = "SELECT COUNT(*) FROM INFOCLIENTES WHERE CEDULA = ?";
    sqlite3_stmt *stmtCheckCedula;

    if (sqlite3_prepare_v2(db, sqlCheckCedula, -1, &stmtCheckCedula, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta para verificar la cédula: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmtCheckCedula, 1, cedula);

    int count = 0;
    if (sqlite3_step(stmtCheckCedula) == SQLITE_ROW) {
        count = sqlite3_column_int(stmtCheckCedula, 0);
    }
    sqlite3_finalize(stmtCheckCedula);

    if (count > 0) {
        cout << "La cédula ingresada ya está registrada. Por lo tanto el cliente ya fue registrado con anterioridad." << endl;
        return;
    }

    // Obtener el ID del cliente más alto en la tabla y generar el siguiente ID
    const char *sqlMaxId = "SELECT MAX(IDCLIENTE) FROM INFOCLIENTES";
    sqlite3_stmt *stmtMaxId;

    if (sqlite3_prepare_v2(db, sqlMaxId, -1, &stmtMaxId, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta para obtener el máximo ID de cliente: " << sqlite3_errmsg(db) << endl;
        return;
    }

    if (sqlite3_step(stmtMaxId) == SQLITE_ROW) {
        idCliente = sqlite3_column_int(stmtMaxId, 0) + 1;
    } else {
        idCliente = 1; // Si no hay registros, el primer ID será 1
    }

    sqlite3_finalize(stmtMaxId);

    cout << "Ingrese el Nombre del Cliente: ";
    cin >> nombre;
    cout << "Ingrese el Apellido del Cliente: ";
    cin >> apellido;

    const char *sql = "INSERT INTO INFOCLIENTES (IDCLIENTE, CEDULA, NOMBRE, APELLIDO) VALUES (?, ?, ?, ?)";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, idCliente);
    sqlite3_bind_int(stmt, 2, cedula);
    sqlite3_bind_text(stmt, 3, nombre.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, apellido.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cerr << "No se pudo agregar el cliente: " << sqlite3_errmsg(db) << endl;
    } else {
        cout << "Cliente " << nombre << " " << apellido << " agregado con exito. El ID del cliente es " << idCliente << "." << endl;
    }

    sqlite3_finalize(stmt);
}
