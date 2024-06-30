#include <iostream>
#include <sqlite3.h>
#include "utilidades.hpp"
#include <cmath>
#include <fstream>
#include <string>

using namespace std;

double convertirMoneda(double monto, const string& monedaOrigen, const string& monedaDestino) {
    const double dolarAColones = 532.0;
    const double colonesADolar = 518.0;

    if ((monedaOrigen == "dolares" || monedaOrigen == "Dolares") && (monedaDestino == "colones" || monedaDestino == "Colones")) {
        return monto * dolarAColones;
    } else if ((monedaOrigen == "colones" || monedaOrigen == "Colones") && (monedaDestino == "dolares" || monedaDestino == "Dolares")) {
        return monto / colonesADolar;
    }
    return monto; // Si las monedas son iguales, no hay conversión
}

void registrarTransaccion(sqlite3* db, int idCuenta, const char* tipoTransaccion, double monto, int idCuentaDestino) {
    const char *sql = "INSERT INTO TRANSACCIONES (ID_CUENTA, ID_CUENTA_DESTINO, TIPO_TRANSACCION, MONTO, FECHA_TRANSACCION) VALUES (?, ?, ?, ?, datetime('now'))";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta de transaccion: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, idCuenta);
    if (idCuentaDestino == -1) {
        sqlite3_bind_null(stmt, 2);
    } else {
        sqlite3_bind_int(stmt, 2, idCuentaDestino);
    }
    sqlite3_bind_text(stmt, 3, tipoTransaccion, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 4, monto);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cerr << "No se pudo registrar la transaccion: " << sqlite3_errmsg(db) << endl;
    } else {
        cout << "Transaccion registrada con exito." << endl;
    }

    sqlite3_finalize(stmt);
}

string mayusculaLetra1(const string& str) {
    if (str.empty()) {
        return str;
    }
    
    string resultado = str;
    resultado[0] = toupper(resultado[0]);
    return resultado;
}