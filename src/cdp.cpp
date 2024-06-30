#include <iostream>
#include <sqlite3.h>

#include "cdp.hpp"

#include <cmath>
#include <fstream>

using namespace std;

void crearCDP(sqlite3* db) {
    int idCliente;
    cout << "Ingrese el ID del Cliente: ";
    cin >> idCliente;

    double monto, interes;
    string fechaInicial, fechaVencimiento;

    // Obtener el nuevo ID de CDP
    int nuevoIdCDP = 0;
    const char *sqlUltimoCDP = "SELECT MAX(ID_CDP) FROM INFO_CDP";
    sqlite3_stmt *stmtUltimoCDP;

    if (sqlite3_prepare_v2(db, sqlUltimoCDP, -1, &stmtUltimoCDP, 0) == SQLITE_OK) {
        if (sqlite3_step(stmtUltimoCDP) == SQLITE_ROW) {
            nuevoIdCDP = sqlite3_column_int(stmtUltimoCDP, 0) + 1;
        } else {
            nuevoIdCDP = 1; // Primer CDP si no hay ninguno
        }
    } else {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmtUltimoCDP);
        return;
    }
    sqlite3_finalize(stmtUltimoCDP);
    while (true) {
    cout << "Ingrese el monto del CDP: ";
    cin >> monto;
if (std::cin.fail()) {
            std::cin.clear(); // Limpiar el estado de error de cin
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignorar la entrada incorrecta
            std::cout << "Entrada inválida. Por favor, introduce un número.\n";
        } else {
            break;
        }
    }



    cout << "Ingrese la tasa de interes: ";
    cin >> interes;
    cout << "Ingrese la fecha inicial (YYYY-MM-DD): ";
    cin >> fechaInicial;
    cout << "Ingrese la fecha de vencimiento (YYYY-MM-DD): ";
    cin >> fechaVencimiento;

    const char *sql = "INSERT INTO INFO_CDP (ID_CDP, ID_CLIENTE, MONTO, INTERES, FECHA_INICIAL, FECHA_VENCIMIENTO) "
                      "VALUES (?, ?, ?, ?, ?, ?)";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, nuevoIdCDP);
    sqlite3_bind_int(stmt, 2, idCliente);
    sqlite3_bind_double(stmt, 3, monto);
    sqlite3_bind_double(stmt, 4, interes);
    sqlite3_bind_text(stmt, 5, fechaInicial.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, fechaVencimiento.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cerr << "No se pudo crear el CDP: " << sqlite3_errmsg(db) << endl;
    } else {
        cout << "CDP creado con exito. El ID de su CDP es " << nuevoIdCDP << "." << endl;
    }

    sqlite3_finalize(stmt);
}


void verCDP(sqlite3* db) {
    int idCliente;
    cout << "Ingrese el ID del Cliente: ";
    cin >> idCliente;

    const char *sql = "SELECT * FROM INFO_CDP WHERE ID_CLIENTE = ?";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, idCliente);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int idCDP = sqlite3_column_int(stmt, 0);
        double monto = sqlite3_column_double(stmt, 2);
        double interes = sqlite3_column_double(stmt, 3);
        const unsigned char* fechaInicial = sqlite3_column_text(stmt, 4);
        const unsigned char* fechaVencimiento = sqlite3_column_text(stmt, 5);
        cout << "ID CDP: " << idCDP << ", Monto: " << monto << ", Interes: " << interes
             << ", Fecha Inicial: " << fechaInicial << ", Fecha de Vencimiento: " << fechaVencimiento << endl;
    }

    sqlite3_finalize(stmt);
}


void eliminarCDP(sqlite3* db) {
    int idCliente;
    cout << "Ingrese el ID del Cliente: ";
    cin >> idCliente;

    int idCDP;
    cout << "Ingrese el ID del CDP que desea eliminar: ";
    cin >> idCDP;

    const char *sql = "DELETE FROM INFO_CDP WHERE ID_CDP = ? AND ID_CLIENTE = ?";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, idCDP);
    sqlite3_bind_int(stmt, 2, idCliente);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cerr << "No se pudo eliminar el CDP: " << sqlite3_errmsg(db) << endl;
    } else {
        cout << "CDP eliminado con exito." << endl;
    }

    sqlite3_finalize(stmt);
}
