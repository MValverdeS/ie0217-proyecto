/**
 * @file  cdp.cpp
 * @brief Se definen las funciones correspondientes a cdp
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

#include "cdp.hpp"

#include <cmath>
#include <fstream>

using namespace std;



/**
 * @brief La función crearCDP es una función tipo void que se encarga de manejar la lógica para la creación de
 * Certificados de Depósito a Plazo (CDP). Requiere que el operario ingrese el ID del cliente, el monto del CDP,
 * la tasa de interés, la fecha inicial y la fecha de vencimiento. Primero, se obtiene el nuevo ID del CDP 
 * consultando el máximo ID actual en la tabla INFO_CDP y sumando 1.Si no hay CDPs existentes, el nuevo ID se 
 * establece en 1. Luego, se solicita al usuario que ingrese el monto,la tasa de interés, la fecha inicial y la 
 * fecha de vencimiento del CDP. Se inserta la nueva información del CDP en la tabla INFO_CDP mediante una consulta SQL. 
 * Si la consulta se ejecuta correctamente, se muestra un mensaje de éxito con el nuevo ID del CDP. Si ocurre algún error, se muestra un mensaje
 * de error correspondiente.
 *
 * @param db puntero a una base de datos de sqlite3, en este caso se utilizará un puntero a la base de datos
 * correspondiente al proyecto.
 */

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


/**
 * @brief La función verCDP es una función tipo void que se encarga de manejar la lógica para mostrar los 
 * Certificados de Depósito a Plazo (CDP) de un cliente específico. Requiere que el usuario ingrese el ID del cliente.
 * Primero, se solicita al usuario que ingrese el ID del cliente. Luego, se prepara una consulta SQL para obtener 
 * todos los CDP asociados con ese cliente en la tabla INFO_CDP. Se enlaza el ID del cliente a la consulta y se 
 * ejecuta. Mientras se obtienen filas de la consulta, se extraen y se muestran los detalles de cada CDP, incluyendo
 * el ID del CDP, el monto, la tasa de interés, la fecha inicial y la fecha de vencimiento. Si ocurre algún error en la 
 * preparación o ejecución de la consulta, se muestra un mensaje de error correspondiente.
 *
 * @param db puntero a una base de datos de sqlite3, en este caso se utilizará un puntero a la base de datos 
 * correspondiente al proyecto.
 */
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


/**
 * @brief La función eliminarCDP es una función tipo void que se encarga de manejar la lógica para eliminar un 
 * Certificado de Depósito a Plazo (CDP) de un cliente específico. Requiere que el usuario ingrese el ID del cliente 
 * y el ID del CDP a eliminar. Primero, se solicita al usuario que ingrese el ID del cliente y el ID del CDP que desea 
 * eliminar. Luego, se prepara una consulta SQL para eliminar el CDP correspondiente de la tabla INFO_CDP, asegurándose de 
 * que coincida tanto el ID del CDP como el ID del cliente. Se enlazan los valores ingresados a la consulta y se ejecuta. Si la 
 * eliminación se realiza con éxito, se muestra un mensaje confirmando la eliminación. Si ocurre algún error en la preparación o 
 * ejecución de la consulta, se muestra un mensaje de error correspondiente.
 *
 * @param db Puntero a una base de datos de sqlite3, en este caso se utilizará un puntero a la base de datos 
 * correspondiente al proyecto.
 */
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
