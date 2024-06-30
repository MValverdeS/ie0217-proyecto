/**
 * @file  prestamos.cpp
 * @brief Se definen las funciones asociadas con utilidades
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
#include "utilidades.hpp"
#include <cmath>
#include <fstream>
#include <string>

using namespace std;


/**
* @brief función tipo double que utiliza operaciones matemáticas para calcular el valor
* de conversión entre colones y dólares.
*
*
* @param monto es un parametro de tipo dobule que corresponde al monto a convertir
* 
* @param monedaOrigen es un parámetro de tipo string que corresponde a la moneda de origen
*
* @param monedaDestino es un parámetro tipo string que corresponde a la moneda destino
*
*
*/


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


/**
* @brief Esta función utiliza los argumentos recibidos para montar una consulta SQL en la tabla transacciones incluyendo 
* la fecha actual utilizando datetime, se tiene un puntero stmt que se utiliza para manejar la declaración preparada, esto 
* este funciona de manera similar a la función descrita anteriormente, además se utiliza sqlite3_prepare de manera similar 
* a la función anterior, se realiza el enlazamiento de parámetros con los argumentos ?, se ejecuta la consulta con sqlite3_step 
* y si es exitosa la transacción se registra con éxito, por último, se utliza sqlite3 finalize para liberar los recursos.
*
*
* @param db corresponde a un puntero hacia una base de datos de sqlite3, en este caso se utilizará un puntero a la base de datos
* correspondiente al proyecto
* 
* @param idCuenta corresponde al ID de la cuenta de origen, es un valor entero
*
* @param tipoTransaccion tipo de transacción corresponde a un puntero hacia el tipo de transacción
*
* @param monto es el montro de la transacción, está en formato double 
*
* @param idCuentaDestino es el id de la cuenta destino
*
*/
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