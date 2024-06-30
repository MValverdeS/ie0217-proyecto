#include <iostream>
#include <cmath>
#include <fstream>
#include <sqlite3.h>

using namespace std;

// Función de callback para imprimir los resultados de las consultas
static int callback(void *data, int argc, char **argv, char **azColName) {
    for (int i = 0; i < argc; i++) {
        cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << endl;
    }
    return 0;
}


void menuInicial() {
    cout << "Bienvenido al Sistema de Gestion Bancaria" << endl;
    cout << "Seleccione el modo de operacion:" << endl;
    cout << "1. Atencion a Clientes" << endl;
    cout << "2. Informacion General sobre Prestamos" << endl;
    cout << "3. Salir" << endl;
}


void menuAtencionClientes() {
    cout << "Menu de Atencion a Clientes" << endl;
    cout << "1. Ver Cuentas" << endl;
    cout << "2. Realizar Deposito" << endl;
    cout << "3. Realizar Retiro" << endl;
    cout << "4. Realizar Transferencia" << endl;
    cout << "5. Prestamos" << endl;
    cout << "6. CDP" << endl;
    cout << "7. Ver Transacciones" << endl;
    cout << "8. Agregar Cliente" << endl;
    cout << "9. Crear Cuenta" << endl;
    cout << "10. Salir" << endl;
}

void menuInformacionPrestamos() {
    cout << "Menu de Informacion General sobre Prestamos" << endl;
    cout << "1. Ver Informacion de Prestamos" << endl;
    cout << "2. Calcular Tabla de Pagos" << endl;
    cout << "3. Agregar Tipo de Prestamo" << endl;
    cout << "4. Salir" << endl;
}

void menuPrestamos() {
    cout << "Menu de Prestamos" << endl;
    cout << "1. Solicitar Prestamo" << endl;
    cout << "2. Ver Prestamos" << endl;
    cout << "3. Generar Reporte de Prestamos" << endl;
    cout << "4. Pagar Prestamo" << endl;
    cout << "5. Salir" << endl;
}

void menuCDP() {
    cout << "Menu de CDP" << endl;
    cout << "1. Crear CDP" << endl;
    cout << "2. Ver CDP" << endl;
    cout << "3. Eliminar CDP" << endl;
    cout << "4. Salir" << endl;

}

void verCuentas(sqlite3* db) {
    int idCliente;
    cout << "Ingrese el ID del Cliente: ";
    cin >> idCliente;
    const char *sql = "SELECT * FROM CUENTAS WHERE ID_CLIENTE = ?";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, idCliente);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int idCuenta = sqlite3_column_int(stmt, 0);
        const unsigned char* tipo = sqlite3_column_text(stmt, 2);
        double monto = sqlite3_column_double(stmt, 3);
        cout << "ID Cuenta: " << idCuenta << ", Tipo: " << tipo << ", Monto: " << monto << endl;
    }

    sqlite3_finalize(stmt);
}

void registrarTransaccion(sqlite3* db, int idCuenta, const char* tipoTransaccion, double monto, int idCuentaDestino = -1) {
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

void verTransacciones(sqlite3* db) {
    int idCliente;
    cout << "Ingrese el ID del Cliente: ";
    cin >> idCliente;
    const char *sql = "SELECT * FROM TRANSACCIONES WHERE ID_CUENTA IN (SELECT ID_CUENTA FROM CUENTAS WHERE ID_CLIENTE = ?)";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, idCliente);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int idTransaccion = sqlite3_column_int(stmt, 0);
        int idCuenta = sqlite3_column_int(stmt, 1);
        int idCuentaDestino = sqlite3_column_int(stmt, 2);
        const unsigned char* tipoTransaccion = sqlite3_column_text(stmt, 3);
        double monto = sqlite3_column_double(stmt, 4);
        const unsigned char* fechaTransaccion = sqlite3_column_text(stmt, 5);
        cout << "ID Transaccion: " << idTransaccion << ", ID Cuenta: " << idCuenta
             << ", ID Cuenta Destino: " << idCuentaDestino << ", Tipo: " << tipoTransaccion
             << ", Monto: " << monto << ", Fecha: " << fechaTransaccion << endl;
    }

    sqlite3_finalize(stmt);
}

void realizarDeposito(sqlite3* db) {
    int idCliente;
    cout << "Ingrese el ID del Cliente: ";
    cin >> idCliente;

    int idCuenta;
    cout << "Ingrese el ID de la cuenta en la que desea realizar el deposito: ";
    cin >> idCuenta;

    // Verificar el tipo de moneda de la cuenta
    string tipoCuenta;
    const char *sqlTipoCuenta = "SELECT TIPO FROM CUENTAS WHERE ID_CUENTA = ? AND ID_CLIENTE = ?";
    sqlite3_stmt *stmtTipoCuenta;

    if (sqlite3_prepare_v2(db, sqlTipoCuenta, -1, &stmtTipoCuenta, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmtTipoCuenta, 1, idCuenta);
        sqlite3_bind_int(stmtTipoCuenta, 2, idCliente);

        if (sqlite3_step(stmtTipoCuenta) == SQLITE_ROW) {
            tipoCuenta = reinterpret_cast<const char*>(sqlite3_column_text(stmtTipoCuenta, 0));
            cout << "La cuenta es en " << tipoCuenta << "." << endl;
        } else {
            cerr << "Cuenta no encontrada para el cliente proporcionado." << endl;
            sqlite3_finalize(stmtTipoCuenta);
            return;
        }
    } else {
        cerr << "No se pudo preparar la consulta para verificar el tipo de cuenta: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmtTipoCuenta);
        return;
    }
    sqlite3_finalize(stmtTipoCuenta);

    // Solicitar la moneda en la que se realizará el depósito
    string monedaDeposito;

    while (true) {
        cout << "Desea realizar el deposito en (colones/dolares): ";
        cin >> monedaDeposito;

        // Convertir a minúsculas para evitar problemas de sensibilidad al caso
        for (auto &c : monedaDeposito) {
            c = tolower(c);
        }

        // Verificar la entrada
        if (monedaDeposito == "dolares" || monedaDeposito == "colones") {
            break; // Salir del bucle si la entrada es correcta
        } else {
            std::cout << "Entrada inválida. Por favor, introduce 'dolares' o 'colones'.\n";
            // Limpiar el flujo de entrada en caso de un error
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }


    double monto;
    double monto1;
    while (true) {
    cout << "Ingrese el monto a depositar en " << monedaDeposito << ": ";
    cin >> monto1;
    if (std::cin.fail()) {
            std::cin.clear(); // Limpiar el estado de error de cin
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignorar la entrada incorrecta
            std::cout << "Entrada inválida. Por favor, introduce un número.\n";
        } else {
            break;
        }
    }


    // Convertir el monto si es necesario
    if (tipoCuenta != monedaDeposito) {
        monto = convertirMoneda(monto1, monedaDeposito, tipoCuenta);
        cout << "El monto convertido a " << tipoCuenta << " es: " << monto << endl;
    }

    // Actualizar el monto en la cuenta
    const char *sql = "UPDATE CUENTAS SET MONTO = MONTO + ? WHERE ID_CUENTA = ? AND ID_CLIENTE = ?";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_double(stmt, 1, monto);
    sqlite3_bind_int(stmt, 2, idCuenta);
    sqlite3_bind_int(stmt, 3, idCliente);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cerr << "No se pudo realizar el deposito: " << sqlite3_errmsg(db) << endl;
    } else {
        cout << "Deposito realizado con exito." << endl;

        // Obtener el nuevo ID de transacción
        int nuevoIdTransaccion = 0;
        const char *sqlUltimaTransaccion = "SELECT MAX(ID_TRANSACCION) FROM TRANSACCIONES";
        sqlite3_stmt *stmtUltimaTransaccion;

        if (sqlite3_prepare_v2(db, sqlUltimaTransaccion, -1, &stmtUltimaTransaccion, 0) == SQLITE_OK) {
            if (sqlite3_step(stmtUltimaTransaccion) == SQLITE_ROW) {
                nuevoIdTransaccion = sqlite3_column_int(stmtUltimaTransaccion, 0) + 1;
            } else {
                nuevoIdTransaccion = 1; // Primera transacción si no hay ninguna
            }
        } else {
            cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmtUltimaTransaccion);
            return;
        }
        sqlite3_finalize(stmtUltimaTransaccion);

        // Registrar la transacción
        const char *sqlRegistrarTransaccion = "INSERT INTO TRANSACCIONES (ID_TRANSACCION, ID_CUENTA, ID_CUENTA_DESTINO, TIPO_TRANSACCION, MONTO, FECHA_TRANSACCION) VALUES (?, ?, ?, ?, ?, datetime('now'))";
        sqlite3_stmt *stmtRegistrarTransaccion;

        if (sqlite3_prepare_v2(db, sqlRegistrarTransaccion, -1, &stmtRegistrarTransaccion, 0) == SQLITE_OK) {
            sqlite3_bind_int(stmtRegistrarTransaccion, 1, nuevoIdTransaccion);
            sqlite3_bind_int(stmtRegistrarTransaccion, 2, idCuenta);
            sqlite3_bind_int(stmtRegistrarTransaccion, 3, idCuenta); // Para depósitos, ID_CUENTA y ID_CUENTA_DESTINO son iguales
            sqlite3_bind_text(stmtRegistrarTransaccion, 4, "Deposito", -1, SQLITE_STATIC);
            sqlite3_bind_double(stmtRegistrarTransaccion, 5, monto);

            if (sqlite3_step(stmtRegistrarTransaccion) != SQLITE_DONE) {
                cerr << "No se pudo registrar la transaccion: " << sqlite3_errmsg(db) << endl;
            } else {
                cout << "Transaccion registrada con exito. El ID de la transaccion es " << nuevoIdTransaccion << "." << endl;
            }
        } else {
            cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        }

        sqlite3_finalize(stmtRegistrarTransaccion);
    }

    sqlite3_finalize(stmt);
}

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

void realizarRetiro(sqlite3* db) {
    int idCliente;
    cout << "Ingrese el ID del Cliente: ";
    cin >> idCliente;

    int idCuenta;
    double monto;
    cout << "Ingrese el ID de la cuenta desde la que desea realizar el retiro: ";
    cin >> idCuenta;

    // Verificar si la cuenta pertenece al cliente
    const char *verificarCuentaSql = "SELECT MONTO, TIPO FROM CUENTAS WHERE ID_CUENTA = ? AND ID_CLIENTE = ?";
    sqlite3_stmt *stmtCuenta;
    if (sqlite3_prepare_v2(db, verificarCuentaSql, -1, &stmtCuenta, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmtCuenta, 1, idCuenta);
    sqlite3_bind_int(stmtCuenta, 2, idCliente);

    double saldoActual;
    string tipoCuenta;
    if (sqlite3_step(stmtCuenta) == SQLITE_ROW) {
        saldoActual = sqlite3_column_double(stmtCuenta, 0);
        tipoCuenta = reinterpret_cast<const char*>(sqlite3_column_text(stmtCuenta, 1));
        cout << "Su cuenta es en " << (tipoCuenta == "Dolares" ? "dólares." : "colones.") << endl;
    } else {
        cerr << "La cuenta no le pertenece." << endl;
        sqlite3_finalize(stmtCuenta);
        return;
    }
    sqlite3_finalize(stmtCuenta);

while (true) {
    cout << "Ingrese el monto a retirar: ";
    cin >> monto;
    if (std::cin.fail()) {
            std::cin.clear(); // Limpiar el estado de error de cin
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignorar la entrada incorrecta
            std::cout << "Entrada inválida. Por favor, introduce un número.\n";
        } else {
            break;
        }
    }


    if (saldoActual < monto) {
        cerr << "Saldo insuficiente." << endl;
        return;
    }

    // Actualizar saldo de la cuenta
    const char *sql = "UPDATE CUENTAS SET MONTO = MONTO - ? WHERE ID_CUENTA = ? AND ID_CLIENTE = ?";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_double(stmt, 1, monto);
    sqlite3_bind_int(stmt, 2, idCuenta);
    sqlite3_bind_int(stmt, 3, idCliente);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cerr << "No se pudo realizar el retiro: " << sqlite3_errmsg(db) << endl;
    } else {
        cout << "Retiro realizado con éxito." << endl;
    }
    sqlite3_finalize(stmt);

    // Registrar la transacción
    int nuevoIdTransaccion = 0;
    const char *sqlUltimaTransaccion = "SELECT MAX(ID_TRANSACCION) FROM TRANSACCIONES";
    sqlite3_stmt *stmtUltimaTransaccion;

    if (sqlite3_prepare_v2(db, sqlUltimaTransaccion, -1, &stmtUltimaTransaccion, 0) == SQLITE_OK) {
        if (sqlite3_step(stmtUltimaTransaccion) == SQLITE_ROW) {
            nuevoIdTransaccion = sqlite3_column_int(stmtUltimaTransaccion, 0) + 1;
        } else {
            nuevoIdTransaccion = 1; // Primera transacción si no hay ninguna
        }
    } else {
        cerr << "No se pudo preparar la consulta para obtener el ID de la última transacción: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmtUltimaTransaccion);
        return;
    }
    sqlite3_finalize(stmtUltimaTransaccion);

    const char *sqlRegistrarTransaccion = "INSERT INTO TRANSACCIONES (ID_TRANSACCION, ID_CUENTA, ID_CUENTA_DESTINO, TIPO_TRANSACCION, MONTO, FECHA_TRANSACCION) VALUES (?, ?, ?, ?, ?, datetime('now'))";
    sqlite3_stmt *stmtRegistrarTransaccion;

    if (sqlite3_prepare_v2(db, sqlRegistrarTransaccion, -1, &stmtRegistrarTransaccion, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta de registro de transacción: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmtRegistrarTransaccion, 1, nuevoIdTransaccion);
    sqlite3_bind_int(stmtRegistrarTransaccion, 2, idCuenta);
    sqlite3_bind_int(stmtRegistrarTransaccion, 3, idCuenta); // ID_CUENTA_DESTINO es el mismo para retiro
    sqlite3_bind_text(stmtRegistrarTransaccion, 4, "Retiro", -1, SQLITE_STATIC);
    sqlite3_bind_double(stmtRegistrarTransaccion, 5, monto);

    if (sqlite3_step(stmtRegistrarTransaccion) != SQLITE_DONE) {
        cerr << "No se pudo registrar la transacción: " << sqlite3_errmsg(db) << endl;
    } else {
        cout << "El retiro del monto fue realizado con éxito. El ID de su transacción es " << nuevoIdTransaccion << "." << endl;
    }

    sqlite3_finalize(stmtRegistrarTransaccion);
}

void realizarTransferencia(sqlite3* db) {
    int idCliente;
    cout << "Ingrese el ID del Cliente: ";
    cin >> idCliente;

    int idCuentaOrigen, idCuentaDestino;
    double monto;
    
    // Verificar si la cuenta de origen pertenece al cliente
    cout << "Ingrese el ID de la cuenta desde la que desea transferir: ";
    cin >> idCuentaOrigen;

    const char *verificarCuentaOrigenSql = "SELECT MONTO, TIPO FROM CUENTAS WHERE ID_CUENTA = ? AND ID_CLIENTE = ?";
    sqlite3_stmt *stmtCuentaOrigen;
    if (sqlite3_prepare_v2(db, verificarCuentaOrigenSql, -1, &stmtCuentaOrigen, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmtCuentaOrigen, 1, idCuentaOrigen);
    sqlite3_bind_int(stmtCuentaOrigen, 2, idCliente);

    double saldoActualOrigen;
    string tipoCuentaOrigen;
    if (sqlite3_step(stmtCuentaOrigen) == SQLITE_ROW) {
        saldoActualOrigen = sqlite3_column_double(stmtCuentaOrigen, 0);
        tipoCuentaOrigen = reinterpret_cast<const char*>(sqlite3_column_text(stmtCuentaOrigen, 1));
        cout << "La cuenta de origen es en " << (tipoCuentaOrigen == "Dolares" ? "dólares." : "colones.") << endl;
    } else {
        cerr << "La cuenta de origen no le pertenece." << endl;
        sqlite3_finalize(stmtCuentaOrigen);
        return;
    }
    sqlite3_finalize(stmtCuentaOrigen);

    // Verificar la cuenta de destino
    cout << "Ingrese el ID de la cuenta de destino: ";
    cin >> idCuentaDestino;

    const char *verificarCuentaDestinoSql = "SELECT TIPO FROM CUENTAS WHERE ID_CUENTA = ?";
    sqlite3_stmt *stmtCuentaDestino;
    if (sqlite3_prepare_v2(db, verificarCuentaDestinoSql, -1, &stmtCuentaDestino, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        return;
    }

    string tipoCuentaDestino;
    if (sqlite3_bind_int(stmtCuentaDestino, 1, idCuentaDestino) == SQLITE_OK && sqlite3_step(stmtCuentaDestino) == SQLITE_ROW) {
        tipoCuentaDestino = reinterpret_cast<const char*>(sqlite3_column_text(stmtCuentaDestino, 0));
        cout << "La cuenta de destino es en " << (tipoCuentaDestino == "Dolares" ? "dólares." : "colones.") << endl;
    } else {
        cerr << "Cuenta de destino no encontrada." << endl;
        sqlite3_finalize(stmtCuentaDestino);
        return;
    }
    sqlite3_finalize(stmtCuentaDestino);

    // Solicitar el monto a transferir

    while (true) {
    cout << "Ingrese el monto a transferir en " << (tipoCuentaOrigen == "Dolares" ? "dólares" : "colones") << ": ";
    cin >> monto;
    if (std::cin.fail()) {
            std::cin.clear(); // Limpiar el estado de error de cin
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignorar la entrada incorrecta
            std::cout << "Entrada inválida. Por favor, introduce un número.\n";
        } else {
            break;
        }
    }
    

    if (saldoActualOrigen < monto) {
        cerr << "Saldo insuficiente." << endl;
        return;
    }

    double montoConvertido = monto;
    if (tipoCuentaOrigen != tipoCuentaDestino) {
        montoConvertido = convertirMoneda(monto, tipoCuentaOrigen, tipoCuentaDestino);
    }

    // Realizar la transferencia
    sqlite3_exec(db, "BEGIN TRANSACTION;", 0, 0, 0);

    const char *actualizarCuentaOrigenSql = "UPDATE CUENTAS SET MONTO = MONTO - ? WHERE ID_CUENTA = ? AND ID_CLIENTE = ?";
    sqlite3_stmt *stmtOrigen;
    if (sqlite3_prepare_v2(db, actualizarCuentaOrigenSql, -1, &stmtOrigen, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        sqlite3_exec(db, "ROLLBACK;", 0, 0, 0);
        return;
    }

    sqlite3_bind_double(stmtOrigen, 1, monto);
    sqlite3_bind_int(stmtOrigen, 2, idCuentaOrigen);
    sqlite3_bind_int(stmtOrigen, 3, idCliente);

    if (sqlite3_step(stmtOrigen) != SQLITE_DONE) {
        cerr << "No se pudo realizar la transferencia: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmtOrigen);
        sqlite3_exec(db, "ROLLBACK;", 0, 0, 0);
        return;
    }
    sqlite3_finalize(stmtOrigen);

    const char *actualizarCuentaDestinoSql = "UPDATE CUENTAS SET MONTO = MONTO + ? WHERE ID_CUENTA = ?";
    sqlite3_stmt *stmtDestino;
    if (sqlite3_prepare_v2(db, actualizarCuentaDestinoSql, -1, &stmtDestino, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        sqlite3_exec(db, "ROLLBACK;", 0, 0, 0);
        return;
    }

    sqlite3_bind_double(stmtDestino, 1, montoConvertido);
    sqlite3_bind_int(stmtDestino, 2, idCuentaDestino);

    if (sqlite3_step(stmtDestino) != SQLITE_DONE) {
        cerr << "No se pudo realizar la transferencia: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmtDestino);
        sqlite3_exec(db, "ROLLBACK;", 0, 0, 0);
        return;
    }
    sqlite3_finalize(stmtDestino);

    sqlite3_exec(db, "COMMIT;", 0, 0, 0);

    // Registrar la transacción
    int nuevoIdTransaccion = 0;
    const char *sqlUltimaTransaccion = "SELECT MAX(ID_TRANSACCION) FROM TRANSACCIONES";
    sqlite3_stmt *stmtUltimaTransaccion;

    if (sqlite3_prepare_v2(db, sqlUltimaTransaccion, -1, &stmtUltimaTransaccion, 0) == SQLITE_OK) {
        if (sqlite3_step(stmtUltimaTransaccion) == SQLITE_ROW) {
            nuevoIdTransaccion = sqlite3_column_int(stmtUltimaTransaccion, 0) + 1;
        } else {
            nuevoIdTransaccion = 1; // Primera transacción si no hay ninguna
        }
    } else {
        cerr << "No se pudo preparar la consulta para obtener el ID de la última transacción: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmtUltimaTransaccion);
        return;
    }
    sqlite3_finalize(stmtUltimaTransaccion);

    const char *sqlRegistrarTransaccion = "INSERT INTO TRANSACCIONES (ID_TRANSACCION, ID_CUENTA, ID_CUENTA_DESTINO, TIPO_TRANSACCION, MONTO, FECHA_TRANSACCION) VALUES (?, ?, ?, ?, ?, datetime('now'))";
    sqlite3_stmt *stmtRegistrarTransaccion;

    if (sqlite3_prepare_v2(db, sqlRegistrarTransaccion, -1, &stmtRegistrarTransaccion, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta de registro de transacción: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmtRegistrarTransaccion, 1, nuevoIdTransaccion);
    sqlite3_bind_int(stmtRegistrarTransaccion, 2, idCuentaOrigen);
    sqlite3_bind_int(stmtRegistrarTransaccion, 3, idCuentaDestino);
    sqlite3_bind_text(stmtRegistrarTransaccion, 4, "Transferencia", -1, SQLITE_STATIC);
    sqlite3_bind_double(stmtRegistrarTransaccion, 5, monto);

    if (sqlite3_step(stmtRegistrarTransaccion) != SQLITE_DONE) {
        cerr << "No se pudo registrar la transacción: " << sqlite3_errmsg(db) << endl;
    } else {
        cout << "La transferencia del monto fue realizada con éxito. El ID de su transacción es " << nuevoIdTransaccion << "." << endl;
    }

    sqlite3_finalize(stmtRegistrarTransaccion);
}

void verInformacionPrestamos(sqlite3* db) {
    const char *sql = "SELECT * FROM TIPOS_PRESTAMO";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int idTipo = sqlite3_column_int(stmt, 0);
        const unsigned char* tipo = sqlite3_column_text(stmt, 1);
        double tasaInteres = sqlite3_column_double(stmt, 2);
        int plazoMeses = sqlite3_column_int(stmt, 3);
        cout << "ID Tipo: " << idTipo << ", Tipo: " << tipo 
             << ", Tasa de Interes: " << tasaInteres << "%, Plazo: " << plazoMeses << " meses" << endl;
    }

    sqlite3_finalize(stmt);
}

void calcularTablaPagos(sqlite3* db) {
    int opcion;
    cout << "Seleccione la opción de cálculo:" << endl;
    cout << "1. Calcular de manera personalizada" << endl;
    cout << "2. Calcular con tipos de préstamo existentes" << endl;
    cin >> opcion;

    if (opcion == 1) {
        double monto, tasaInteres;
        int plazoMeses;

while (true) {
    cout << "Ingrese el monto del préstamo: ";
        cin >> monto;
        cout << "Ingrese la tasa de interés (en %): ";
        cin >> tasaInteres;
        cout << "Ingrese el plazo en meses: ";
        cin >> plazoMeses;
    if (std::cin.fail()) {
            std::cin.clear(); // Limpiar el estado de error de cin
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignorar la entrada incorrecta
            std::cout << "Entrada inválida. Por favor, introduce un número.\n";
        } else {
            break;
        }
    }
        

        tasaInteres = tasaInteres / 100 / 12; // Convertir tasa anual a mensual
        double cuotaMensual = (monto * tasaInteres) / (1 - pow(1 + tasaInteres, -plazoMeses));

        cout << "Cuota mensual: " << cuotaMensual << endl;
        cout << "Tabla de pagos esperados:" << endl;

        double saldo = monto;
        for (int i = 1; i <= plazoMeses; ++i) {
            double interes = saldo * tasaInteres;
            double principal = cuotaMensual - interes;
            saldo -= principal;
            cout << "Mes " << i << ": Pago: " << cuotaMensual 
                << ", Interes: " << interes 
                << ", Principal: " << principal 
                << ", Saldo restante: " << saldo << endl;
        }
    } else if (opcion == 2) {
        // Imprimir los tipos de préstamos disponibles
        const char *sqlTiposPrestamo = "SELECT ID_TIPO, TIPO, TASA_INTERES, PLAZO_MESES FROM TIPOS_PRESTAMO";
        sqlite3_stmt *stmtTipos;

        if (sqlite3_prepare_v2(db, sqlTiposPrestamo, -1, &stmtTipos, 0) != SQLITE_OK) {
            cerr << "No se pudo preparar la consulta para tipos de préstamo: " << sqlite3_errmsg(db) << endl;
            return;
        }

        cout << "Tipos de préstamo disponibles:" << endl;
        while (sqlite3_step(stmtTipos) == SQLITE_ROW) {
            int idTipo = sqlite3_column_int(stmtTipos, 0);
            const unsigned char* tipo = sqlite3_column_text(stmtTipos, 1);
            double tasaInteres = sqlite3_column_double(stmtTipos, 2);
            int plazoMeses = sqlite3_column_int(stmtTipos, 3);
            cout << "ID Tipo: " << idTipo << ", Tipo: " << tipo 
                << ", Tasa de Interes: " << tasaInteres << "%, Plazo: " << plazoMeses << " meses" << endl;
        }

        sqlite3_finalize(stmtTipos);

        // Solicitar el tipo de ID del préstamo
        int tipoId;
        cout << "Ingrese el ID del tipo de préstamo que desea calcular: ";
        cin >> tipoId;

        // Obtener los datos del tipo de préstamo seleccionado
        const char *sqlTipoSeleccionado = "SELECT TIPO, TASA_INTERES, PLAZO_MESES FROM TIPOS_PRESTAMO WHERE ID_TIPO = ?";
        sqlite3_stmt *stmtTipoSeleccionado;

        if (sqlite3_prepare_v2(db, sqlTipoSeleccionado, -1, &stmtTipoSeleccionado, 0) != SQLITE_OK) {
            cerr << "No se pudo preparar la consulta para el tipo de préstamo seleccionado: " << sqlite3_errmsg(db) << endl;
            return;
        }

        sqlite3_bind_int(stmtTipoSeleccionado, 1, tipoId);

        string tipoPrestamo;
        double tasaInteres;
        int plazoMeses;
        if (sqlite3_step(stmtTipoSeleccionado) == SQLITE_ROW) {
            tipoPrestamo = reinterpret_cast<const char*>(sqlite3_column_text(stmtTipoSeleccionado, 0));
            tasaInteres = sqlite3_column_double(stmtTipoSeleccionado, 1);
            plazoMeses = sqlite3_column_int(stmtTipoSeleccionado, 2);
        } else {
            cerr << "Tipo de préstamo no encontrado." << endl;
            sqlite3_finalize(stmtTipoSeleccionado);
            return;
        }
        sqlite3_finalize(stmtTipoSeleccionado);

        // Solicitar la moneda y el monto del préstamo
        string moneda;
        double monto;

        

        while (true) {
        std::cout << "Ingrese el tipo de moneda (dolares o colones): ";
        std::cin >> moneda;

        // Convertir a minúsculas para evitar problemas de sensibilidad al caso
        for (auto &c : moneda) {
            c = tolower(c);
        }

        // Verificar la entrada
        if (moneda == "dolares" || moneda == "colones") {
            break; // Salir del bucle si la entrada es correcta
        } else {
            std::cout << "Entrada inválida. Por favor, introduce 'dolares' o 'colones'.\n";
            // Limpiar el flujo de entrada en caso de un error
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        }

        while (true) {
        cout << "Ingrese el monto del préstamo: ";
        cin >> monto;
    if (std::cin.fail()) {
            std::cin.clear(); // Limpiar el estado de error de cin
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignorar la entrada incorrecta
            std::cout << "Entrada inválida. Por favor, introduce un número.\n";
        } else {
            break;
        }
    }
       

        // Calcular la cuota mensual y la tabla de pagos
        tasaInteres = tasaInteres / 100 / 12; // Convertir tasa anual a mensual
        double cuotaMensual = (monto * tasaInteres) / (1 - pow(1 + tasaInteres, -plazoMeses));

        cout << "Cuota mensual: " << cuotaMensual << endl;
        cout << "Tabla de pagos esperados:" << endl;

        double saldo = monto;
        for (int i = 1; i <= plazoMeses; ++i) {
            double interes = saldo * tasaInteres;
            double principal = cuotaMensual - interes;
            saldo -= principal;
            cout << "Mes " << i << ": Pago: " << cuotaMensual 
                << ", Interes: " << interes 
                << ", Principal: " << principal 
                << ", Saldo restante: " << saldo << endl;
        }
    } else {
        cerr << "Opción no válida. Por favor, intente de nuevo." << endl;
    }
}

void imprimirTiposPrestamo(sqlite3* db) {
    const char *sql = "SELECT * FROM TIPOS_PRESTAMO";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        return;
    }

    cout << "Tipos de Prestamos Disponibles:" << endl;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int idTipo = sqlite3_column_int(stmt, 0);
        const char* tipo = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        double tasaInteres = sqlite3_column_double(stmt, 2);
        int plazoMeses = sqlite3_column_int(stmt, 3);
        cout << "ID Tipo: " << idTipo << ", Tipo: " << tipo 
             << ", Tasa de Interes: " << tasaInteres << "%, Plazo: " << plazoMeses << " meses" << endl;
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

void solicitarPrestamo(sqlite3* db) {
    int idCliente;
    cout << "Ingrese el ID del Cliente: ";
    cin >> idCliente;

    // Imprimir tipos de prestamo disponibles
    imprimirTiposPrestamo(db);

    // Solicitar y verificar el tipo de prestamo
    int tipoId;
    cout << "Ingrese el ID del tipo de prestamo: ";
    cin >> tipoId;

    const char *verificarTipoSql = "SELECT TIPO, TASA_INTERES, PLAZO_MESES FROM TIPOS_PRESTAMO WHERE ID_TIPO = ?";
    sqlite3_stmt *stmtVerificarTipo;

    if (sqlite3_prepare_v2(db, verificarTipoSql, -1, &stmtVerificarTipo, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmtVerificarTipo, 1, tipoId);

    string tipoPrestamo;
    double tasaInteres;
    int plazoMeses;
    if (sqlite3_step(stmtVerificarTipo) == SQLITE_ROW) {
        tipoPrestamo = reinterpret_cast<const char*>(sqlite3_column_text(stmtVerificarTipo, 0));
        tasaInteres = sqlite3_column_double(stmtVerificarTipo, 1);
        plazoMeses = sqlite3_column_int(stmtVerificarTipo, 2);
    } else {
        cerr << "Tipo de prestamo no encontrado." << endl;
        sqlite3_finalize(stmtVerificarTipo);
        return;
    }
    sqlite3_finalize(stmtVerificarTipo);

    // Solicitar la moneda y el monto del prestamo
    string moneda;
    string moneda1;
    double monto;

while (true) {
        std::cout << "Ingrese el tipo de moneda (dolares o colones): ";
        std::cin >> moneda;

        // Convertir a minúsculas para evitar problemas de sensibilidad al caso
        for (auto &c : moneda) {
            c = tolower(c);
        }

        // Verificar la entrada
        if (moneda == "dolares" || moneda == "colones") {
            break; // Salir del bucle si la entrada es correcta
        } else {
            std::cout << "Entrada inválida. Por favor, introduce 'dolares' o 'colones'.\n";
            // Limpiar el flujo de entrada en caso de un error
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
}


    while (true) {
     cout << "Ingrese el monto del prestamo: ";
    cin >> monto;
    if (std::cin.fail()) {
            std::cin.clear(); // Limpiar el estado de error de cin
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignorar la entrada incorrecta
            std::cout << "Entrada inválida. Por favor, introduce un número.\n";
        } else {
            break;
        }
    }
   
    moneda1 = mayusculaLetra1(moneda);
    // Verificar que el cliente tenga una cuenta en la moneda del prestamo
    const char *verificarCuentaSql = "SELECT ID_CUENTA FROM CUENTAS WHERE ID_CLIENTE = ? AND TIPO = ?";
    sqlite3_stmt *stmtVerificarCuenta;

    if (sqlite3_prepare_v2(db, verificarCuentaSql, -1, &stmtVerificarCuenta, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmtVerificarCuenta, 1, idCliente);
    sqlite3_bind_text(stmtVerificarCuenta, 2, moneda1.c_str(), -1, SQLITE_STATIC);

    int idCuenta = 0;
    if (sqlite3_step(stmtVerificarCuenta) == SQLITE_ROW) {
        idCuenta = sqlite3_column_int(stmtVerificarCuenta, 0);
    } else {
        cerr << "El cliente no tiene una cuenta en " << moneda << "." << endl;
        sqlite3_finalize(stmtVerificarCuenta);
        return;
    }
    sqlite3_finalize(stmtVerificarCuenta);

    // Generar automaticamente el ID del prestamo
    int idPrestamo = 0;
    const char *sqlUltimoPrestamo = "SELECT MAX(ID_PRESTAMO) FROM INFO_PRESTAMOS";
    sqlite3_stmt *stmtUltimoPrestamo;

    if (sqlite3_prepare_v2(db, sqlUltimoPrestamo, -1, &stmtUltimoPrestamo, 0) == SQLITE_OK) {
        if (sqlite3_step(stmtUltimoPrestamo) == SQLITE_ROW) {
            idPrestamo = sqlite3_column_int(stmtUltimoPrestamo, 0) + 1;
        } else {
            idPrestamo = 1; // Primer prestamo si no hay ninguno
        }
    } else {
        cerr << "No se pudo preparar la consulta para obtener el ID del ultimo prestamo: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmtUltimoPrestamo);
        return;
    }
    sqlite3_finalize(stmtUltimoPrestamo);

    // Calcular la cuota mensual
    tasaInteres = tasaInteres / 100.0;
    double cuotaMensual = (monto * tasaInteres) / (1 - pow(1 + tasaInteres, -plazoMeses));

    // Insertar el prestamo en la tabla INFO_PRESTAMOS
    const char *sql = "INSERT INTO INFO_PRESTAMOS (ID_PRESTAMO, ID_CLIENTE, TIPO_ID, TIPO_PRESTAMO, MONEDA, MONTO, TASA_INTERES, PLAZO, CUOTA_MENSUAL) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, idPrestamo);
    sqlite3_bind_int(stmt, 2, idCliente);
    sqlite3_bind_int(stmt, 3, tipoId);
    sqlite3_bind_text(stmt, 4, tipoPrestamo.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, moneda.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 6, monto);
    sqlite3_bind_double(stmt, 7, tasaInteres);
    sqlite3_bind_int(stmt, 8, plazoMeses);
    sqlite3_bind_double(stmt, 9, cuotaMensual);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cerr << "No se pudo insertar el prestamo: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);

    // Actualizar el monto de la cuenta correspondiente
    const char *sqlActualizarCuenta = "UPDATE CUENTAS SET MONTO = MONTO + ? WHERE ID_CUENTA = ?";
    sqlite3_stmt *stmtActualizarCuenta;

    if (sqlite3_prepare_v2(db, sqlActualizarCuenta, -1, &stmtActualizarCuenta, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_double(stmtActualizarCuenta, 1, monto);
    sqlite3_bind_int(stmtActualizarCuenta, 2, idCuenta);

    if (sqlite3_step(stmtActualizarCuenta) != SQLITE_DONE) {
        cerr << "No se pudo actualizar el monto de la cuenta: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmtActualizarCuenta);
        return;
    }
    sqlite3_finalize(stmtActualizarCuenta);

    cout << "El prestamo por " << monto << " en " << moneda << " fue realizado con exito. El ID de su prestamo es " << idPrestamo << "." << endl;
}

void verPrestamos(sqlite3* db) {
    int idCliente;
    cout << "Ingrese el ID del Cliente: ";
    cin >> idCliente;

    const char *sql = "SELECT * FROM INFO_PRESTAMOS WHERE ID_CLIENTE = ?";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, idCliente);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int idPrestamo = sqlite3_column_int(stmt, 0);
        const unsigned char* tipoPrestamo = sqlite3_column_text(stmt, 3);
        const unsigned char* moneda = sqlite3_column_text(stmt, 4);
        double monto = sqlite3_column_double(stmt, 5);
        double tasaInteres = sqlite3_column_double(stmt, 6);
        int plazo = sqlite3_column_int(stmt, 7);
        double cuotaMensual = sqlite3_column_double(stmt, 8);
        cout << "ID Prestamo: " << idPrestamo << ", Tipo: " << tipoPrestamo 
             << ", Moneda: " << moneda << ", Monto: " << monto 
             << ", Tasa de Interes: " << tasaInteres << "%, Plazo: " << plazo 
             << " meses, Cuota Mensual: " << cuotaMensual << endl;
    }

    sqlite3_finalize(stmt);
}


void generarReportePrestamos(sqlite3* db) {
    int idCliente;
    cout << "Ingrese el ID del Cliente: ";
    cin >> idCliente;

    const char *sql = "SELECT * FROM INFO_PRESTAMOS WHERE ID_CLIENTE = ?";
    sqlite3_stmt *stmt;
    ofstream reporte("reporte_prestamos.txt");

    if (!reporte.is_open()) {
        cerr << "No se pudo abrir el archivo de reporte." << endl;
        return;
    }

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, idCliente);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int idPrestamo = sqlite3_column_int(stmt, 0);
        const unsigned char* tipoPrestamo = sqlite3_column_text(stmt, 3);
        const unsigned char* moneda = sqlite3_column_text(stmt, 4);
        double monto = sqlite3_column_double(stmt, 5);
        double tasaInteres = sqlite3_column_double(stmt, 6);
        int plazo = sqlite3_column_int(stmt, 7);
        double cuotaMensual = sqlite3_column_double(stmt, 8);
        reporte << "ID Prestamo: " << idPrestamo << "\nTipo: " << tipoPrestamo 
                << "\nMoneda: " << moneda << "\nMonto: " << monto 
                << "\nTasa de Interes: " << tasaInteres << "%\nPlazo: " << plazo 
                << " meses\nCuota Mensual: " << cuotaMensual << "\n\n";
    }

    sqlite3_finalize(stmt);
    reporte.close();
    cout << "Reporte de prestamos generado con exito en 'reporte_prestamos.txt'." << endl;
}

void pagarPrestamo(sqlite3* db) {
    int idCliente;
    cout << "Ingrese el ID del Cliente: ";
    cin >> idCliente;

    // Verificar que el cliente exista
    const char *sqlVerificarCliente = "SELECT COUNT(*) FROM INFOCLIENTES WHERE IDCLIENTE = ?";
    sqlite3_stmt *stmtVerificarCliente;

    if (sqlite3_prepare_v2(db, sqlVerificarCliente, -1, &stmtVerificarCliente, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta para verificar el cliente: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmtVerificarCliente, 1, idCliente);

    int clienteExiste = 0;
    if (sqlite3_step(stmtVerificarCliente) == SQLITE_ROW) {
        clienteExiste = sqlite3_column_int(stmtVerificarCliente, 0);
    }
    sqlite3_finalize(stmtVerificarCliente);

    if (clienteExiste == 0) {
        cout << "El cliente no existe." << endl;
        return;
    }

    int idPrestamo;
    double montoPago;
    double montoPagoOriginal;

    cout << "Ingrese el ID del prestamo que desea pagar: ";
    cin >> idPrestamo;

    // Obtener información del préstamo
    const char *sqlInfoPrestamo = "SELECT MONTO, TASA_INTERES, PLAZO, MONEDA FROM INFO_PRESTAMOS WHERE ID_PRESTAMO = ? AND ID_CLIENTE = ?";
    sqlite3_stmt *stmtInfo;
    if (sqlite3_prepare_v2(db, sqlInfoPrestamo, -1, &stmtInfo, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta de información del préstamo: " << sqlite3_errmsg(db) << endl;
        return;
    }
    sqlite3_bind_int(stmtInfo, 1, idPrestamo);
    sqlite3_bind_int(stmtInfo, 2, idCliente);

    double montoRestante, tasaInteres;
    int plazo;
    string monedaPrestamo;
    if (sqlite3_step(stmtInfo) == SQLITE_ROW) {
        montoRestante = sqlite3_column_double(stmtInfo, 0);
        tasaInteres = sqlite3_column_double(stmtInfo, 1);
        plazo = sqlite3_column_int(stmtInfo, 2);
        monedaPrestamo = reinterpret_cast<const char*>(sqlite3_column_text(stmtInfo, 3));
        cout << "El préstamo está en " << monedaPrestamo << "." << endl;
    } else {
        cerr << "Préstamo no encontrado." << endl;
        sqlite3_finalize(stmtInfo);
        return;
    }
    sqlite3_finalize(stmtInfo);

    // Solicitar ID de la cuenta para el pago y verificar que pertenezca al cliente
    int idCuenta;
    cout << "Ingrese el ID de la cuenta con la que desea pagar: ";
    cin >> idCuenta;

    const char *verificarCuentaSql = "SELECT TIPO, MONTO FROM CUENTAS WHERE ID_CUENTA = ? AND ID_CLIENTE = ?";
    sqlite3_stmt *stmtVerificarCuenta;

    if (sqlite3_prepare_v2(db, verificarCuentaSql, -1, &stmtVerificarCuenta, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta de verificación de cuenta: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmtVerificarCuenta, 1, idCuenta);
    sqlite3_bind_int(stmtVerificarCuenta, 2, idCliente);

    string monedaCuenta;
    double saldoCuenta;
    if (sqlite3_step(stmtVerificarCuenta) == SQLITE_ROW) {
        monedaCuenta = reinterpret_cast<const char*>(sqlite3_column_text(stmtVerificarCuenta, 0));
        saldoCuenta = sqlite3_column_double(stmtVerificarCuenta, 1);
        cout << "La cuenta está en " << monedaCuenta << "." << endl;
    } else {
        cerr << "Cuenta no encontrada o no pertenece al cliente." << endl;
        sqlite3_finalize(stmtVerificarCuenta);
        return;
    }
    sqlite3_finalize(stmtVerificarCuenta);

while (true) {
    cout << "Ingrese el monto a pagar en " << monedaCuenta << ": ";
    cin >> montoPago;
    if (std::cin.fail()) {
            std::cin.clear(); // Limpiar el estado de error de cin
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignorar la entrada incorrecta
            std::cout << "Entrada inválida. Por favor, introduce un número.\n";
        } else {
            break;
        }
    }
    
    montoPagoOriginal = montoPago;

    if (saldoCuenta < montoPago) {
        cerr << "Saldo insuficiente en la cuenta." << endl;
        return;
    }

    // Convertir monto si las monedas son diferentes
    if (monedaPrestamo != monedaCuenta) {
        montoPago = convertirMoneda(montoPago, monedaCuenta, monedaPrestamo);
    }

    // Verificar que el monto a pagar es suficiente para la cuota
    double pagoInteres = montoRestante * (tasaInteres / 100 / 12);
    double pagoCapital = montoPago - pagoInteres;

    if (pagoCapital <= 0) {
        cerr << "El monto ingresado no es suficiente para cubrir los intereses del préstamo." << endl;
        return;
    }

    montoRestante -= pagoCapital;

    // Actualizar el monto del préstamo
    const char *sqlActualizarPrestamo = "UPDATE INFO_PRESTAMOS SET MONTO = ? WHERE ID_PRESTAMO = ? AND ID_CLIENTE = ?";
    sqlite3_stmt *stmtActualizar;
    if (sqlite3_prepare_v2(db, sqlActualizarPrestamo, -1, &stmtActualizar, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta de actualización del préstamo: " << sqlite3_errmsg(db) << endl;
        return;
    }
    sqlite3_bind_double(stmtActualizar, 1, montoRestante);
    sqlite3_bind_int(stmtActualizar, 2, idPrestamo);
    sqlite3_bind_int(stmtActualizar, 3, idCliente);

    if (sqlite3_step(stmtActualizar) != SQLITE_DONE) {
        cerr << "No se pudo actualizar el préstamo: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmtActualizar);
        return;
    }
    sqlite3_finalize(stmtActualizar);

    // Actualizar el saldo de la cuenta
    const char *sqlActualizarCuenta = "UPDATE CUENTAS SET MONTO = MONTO - ? WHERE ID_CUENTA = ? AND ID_CLIENTE = ?";
    sqlite3_stmt *stmtActualizarCuenta;

    if (sqlite3_prepare_v2(db, sqlActualizarCuenta, -1, &stmtActualizarCuenta, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta de actualización de la cuenta: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_double(stmtActualizarCuenta, 1, montoPagoOriginal);
    sqlite3_bind_int(stmtActualizarCuenta, 2, idCuenta);
    sqlite3_bind_int(stmtActualizarCuenta, 3, idCliente);

    if (sqlite3_step(stmtActualizarCuenta) != SQLITE_DONE) {
        cerr << "No se pudo actualizar el saldo de la cuenta: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmtActualizarCuenta);
        return;
    }
    sqlite3_finalize(stmtActualizarCuenta);

    // Generar automáticamente el ID del pago
    int idPago = 0;
    const char *sqlUltimoPago = "SELECT MAX(ID_PAGO) FROM PAGO_PRESTAMOS";
    sqlite3_stmt *stmtUltimoPago;

    if (sqlite3_prepare_v2(db, sqlUltimoPago, -1, &stmtUltimoPago, 0) == SQLITE_OK) {
        if (sqlite3_step(stmtUltimoPago) == SQLITE_ROW) {
            idPago = sqlite3_column_int(stmtUltimoPago, 0) + 1;
        } else {
            idPago = 1; // Primer pago si no hay ninguno
        }
    } else {
        cerr << "No se pudo preparar la consulta para obtener el ID del último pago: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmtUltimoPago);
        return;
    }
    sqlite3_finalize(stmtUltimoPago);

    // Registrar el pago en la tabla de pagos
    const char *sqlRegistrarPago = "INSERT INTO PAGO_PRESTAMOS (ID_PAGO, ID_PRESTAMO, MONTO, PAGO_CAPITAL, PAGO_INTERES, FECHA_PAGO) "
                                   "VALUES (?, ?, ?, ?, ?, datetime('now'))";
    sqlite3_stmt *stmtPago;
    if (sqlite3_prepare_v2(db, sqlRegistrarPago, -1, &stmtPago, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta de registro de pago: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmtPago, 1, idPago);
    sqlite3_bind_int(stmtPago, 2, idPrestamo);
    sqlite3_bind_double(stmtPago, 3, montoPago);
    sqlite3_bind_double(stmtPago, 4, pagoCapital);
    sqlite3_bind_double(stmtPago, 5, pagoInteres);

    if (sqlite3_step(stmtPago) != SQLITE_DONE) {
        cerr << "No se pudo registrar el pago: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmtPago);
        return;
    } else {
        cout << "Pago del préstamo " << idPrestamo << " realizado con éxito. El ID de su pago de préstamo es " << idPago << "." << endl;
    }

    sqlite3_finalize(stmtPago);
}

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

void agregarTipoPrestamo(sqlite3* db) {
    int idTipo, plazoMeses;
    double tasaInteres;
    string tipo;

    cout << "Ingrese el ID del tipo de prestamo: ";
    cin >> idTipo;
    cout << "Ingrese el tipo de prestamo: ";
    cin >> tipo;
    cout << "Ingrese la tasa de interes (en %): ";
    cin >> tasaInteres;
    cout << "Ingrese el plazo en meses: ";
    cin >> plazoMeses;

    const char *sql = "INSERT INTO TIPOS_PRESTAMO (ID_TIPO, TIPO, TASA_INTERES, PLAZO_MESES) VALUES (?, ?, ?, ?)";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, idTipo);
    sqlite3_bind_text(stmt, 2, tipo.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, tasaInteres);
    sqlite3_bind_int(stmt, 4, plazoMeses);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cerr << "No se pudo agregar el tipo de prestamo: " << sqlite3_errmsg(db) << endl;
    } else {
        cout << "Tipo de prestamo agregado con exito." << endl;
    }

    sqlite3_finalize(stmt);
}

void crearCuenta(sqlite3* db) {
    int cedula;
    cout << "Ingrese la cedula del Cliente: ";
    cin >> cedula;

    // Buscar el ID del cliente usando la cedula
    int idCliente = -1;
    const char *sqlBuscarCliente = "SELECT IDCLIENTE FROM INFOCLIENTES WHERE CEDULA = ?";
    sqlite3_stmt *stmtBuscarCliente;

    if (sqlite3_prepare_v2(db, sqlBuscarCliente, -1, &stmtBuscarCliente, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmtBuscarCliente, 1, cedula);

        if (sqlite3_step(stmtBuscarCliente) == SQLITE_ROW) {
            idCliente = sqlite3_column_int(stmtBuscarCliente, 0);
        } else {
            cerr << "Cliente no encontrado." << endl;
            sqlite3_finalize(stmtBuscarCliente);
            return;
        }
    } else {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        return;
    }
    sqlite3_finalize(stmtBuscarCliente);

    // Verificar cuántas cuentas tiene el cliente
    const char *sqlContarCuentas = "SELECT COUNT(*) FROM CUENTAS WHERE ID_CLIENTE = ?";
    sqlite3_stmt *stmtContarCuentas;
    int cuentaCount = 0;

    if (sqlite3_prepare_v2(db, sqlContarCuentas, -1, &stmtContarCuentas, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmtContarCuentas, 1, idCliente);

        if (sqlite3_step(stmtContarCuentas) == SQLITE_ROW) {
            cuentaCount = sqlite3_column_int(stmtContarCuentas, 0);
        }
    } else {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmtContarCuentas);
        return;
    }
    sqlite3_finalize(stmtContarCuentas);

    if (cuentaCount >= 2) {
        cerr << "El cliente ya tiene el máximo número de cuentas permitidas." << endl;
        return;
    }

    // Obtener el nuevo ID de cuenta
    int nuevoIdCuenta = 0;
    const char *sqlUltimaCuenta = "SELECT MAX(ID_CUENTA) FROM CUENTAS";
    sqlite3_stmt *stmtUltimaCuenta;

    if (sqlite3_prepare_v2(db, sqlUltimaCuenta, -1, &stmtUltimaCuenta, 0) == SQLITE_OK) {
        if (sqlite3_step(stmtUltimaCuenta) == SQLITE_ROW) {
            nuevoIdCuenta = sqlite3_column_int(stmtUltimaCuenta, 0) + 1;
        } else {
            nuevoIdCuenta = 1; // Primera cuenta si no hay ninguna
        }
    } else {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmtUltimaCuenta);
        return;
    }
    sqlite3_finalize(stmtUltimaCuenta);

    // Solicitar tipo de cuenta (dólares o colones)
    string tipoCuenta;

    while (true) {
        cout << "Ingrese el tipo de cuenta (Dolares/Colones): ";
        cin >> tipoCuenta;

        // Convertir a minúsculas para evitar problemas de sensibilidad al caso
        for (auto &c : tipoCuenta) {
            c = tolower(c);
        }

        // Verificar la entrada
        if (tipoCuenta == "dolares" || tipoCuenta == "colones") {
            break; // Salir del bucle si la entrada es correcta
        } else {
            std::cout << "Entrada inválida. Por favor, introduce 'dolares' o 'colones'.\n";
            // Limpiar el flujo de entrada en caso de un error
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    

    if (tipoCuenta != "Dolares" && tipoCuenta != "Colones") {
        cerr << "Tipo de cuenta no válido." << endl;
        return;
    }

    // Solicitar monto inicial
    double montoInicial;

    while (true) {
    cout << "Ingrese el monto inicial para la cuenta en " << tipoCuenta << ": ";
    cin >> montoInicial;
    if (std::cin.fail()) {
            std::cin.clear(); // Limpiar el estado de error de cin
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignorar la entrada incorrecta
            std::cout << "Entrada inválida. Por favor, introduce un número.\n";
        } else {
            break;
        }
    }
   

    // Crear la cuenta
    const char *sqlCrearCuenta = "INSERT INTO CUENTAS (ID_CUENTA, ID_CLIENTE, TIPO, MONTO) VALUES (?, ?, ?, ?)";
    sqlite3_stmt *stmtCrearCuenta;

    if (sqlite3_prepare_v2(db, sqlCrearCuenta, -1, &stmtCrearCuenta, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmtCrearCuenta, 1, nuevoIdCuenta);
        sqlite3_bind_int(stmtCrearCuenta, 2, idCliente);
        sqlite3_bind_text(stmtCrearCuenta, 3, tipoCuenta.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_double(stmtCrearCuenta, 4, montoInicial);

        if (sqlite3_step(stmtCrearCuenta) == SQLITE_DONE) {
            cout << "Cuenta creada con éxito, su cuenta en " << tipoCuenta << " tiene el siguiente ID: " << nuevoIdCuenta << endl;
        } else {
            cerr << "No se pudo crear la cuenta: " << sqlite3_errmsg(db) << endl;
        }
    } else {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmtCrearCuenta);
}



int main(int argc, char* argv[]) {
    sqlite3 *db;
    char *errMsg = 0;
    int rc;
    const char *data = "Callback function called";

    // Abre la base de datos del banco (la crea si no existe)
    rc = sqlite3_open("banco.db", &db);
    if (rc) {
        cerr << "No se pudo abrir la base de datos: " << sqlite3_errmsg(db) << endl;
        return(0);
    } else {
        cout << "Base de datos abierta con éxito" << endl;
    }

    // Activa las claves foráneas
    const char *foreignKeysSql = "PRAGMA foreign_keys = ON;";
    rc = sqlite3_exec(db, foreignKeysSql, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    }

    // Crear las tablas
    const char *tablaInfoClientes = "CREATE TABLE IF NOT EXISTS INFOCLIENTES ("
                                             "IDCLIENTE INT PRIMARY KEY NOT NULL, "
                                             "CEDULA INT NOT NULL, "
                                             "NOMBRE TEXT NOT NULL, "
                                             "APELLIDO TEXT NOT NULL);";
    rc = sqlite3_exec(db, tablaInfoClientes, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Tabla INFOCLIENTES creada con éxito" << endl;
    }

    // Crear índice en CEDULA
    const char *indiceCedula = "CREATE INDEX IF NOT EXISTS idx_cedula ON INFOCLIENTES(CEDULA);";
    rc = sqlite3_exec(db, indiceCedula, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error al crear índice: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Índice en CEDULA creado con éxito" << endl;
    }

    const char *tablaTiposPrestamo = "CREATE TABLE IF NOT EXISTS TIPOS_PRESTAMO ("
                                              "ID_TIPO INT PRIMARY KEY NOT NULL, "
                                              "TIPO TEXT NOT NULL, "
                                              "TASA_INTERES REAL NOT NULL, "
                                              "PLAZO_MESES INT NOT NULL);";
    rc = sqlite3_exec(db, tablaTiposPrestamo, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Tabla TIPOS_PRESTAMO creada con éxito" << endl;
    }

    const char *tablaCuentas = "CREATE TABLE IF NOT EXISTS CUENTAS ("
                                        "ID_CUENTA INT PRIMARY KEY NOT NULL, "
                                        "ID_CLIENTE INT NOT NULL, "
                                        "TIPO TEXT NOT NULL, "
                                        "MONTO REAL NOT NULL, "
                                        "FOREIGN KEY(ID_CLIENTE) REFERENCES INFOCLIENTES(IDCLIENTE));";
    rc = sqlite3_exec(db, tablaCuentas, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Tabla CUENTAS creada con éxito" << endl;
    }

    const char *tablaInfoPrestamos = "CREATE TABLE IF NOT EXISTS INFO_PRESTAMOS ("
                                              "ID_PRESTAMO INT PRIMARY KEY NOT NULL, "
                                              "ID_CLIENTE INT NOT NULL, "
                                              "TIPO_ID INT NOT NULL, "
                                              "TIPO_PRESTAMO TEXT NOT NULL, "
                                              "MONEDA TEXT NOT NULL, "
                                              "MONTO REAL NOT NULL, "
                                              "TASA_INTERES REAL NOT NULL, "
                                              "PLAZO INT NOT NULL, "
                                              "CUOTA_MENSUAL REAL NOT NULL, "
                                              "FOREIGN KEY(ID_CLIENTE) REFERENCES INFOCLIENTES(IDCLIENTE), "
                                              "FOREIGN KEY(TIPO_ID) REFERENCES TIPOS_PRESTAMO(ID_TIPO));";
    rc = sqlite3_exec(db, tablaInfoPrestamos, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Tabla INFO_PRESTAMOS creada con éxito" << endl;
    }

    const char *tablaInfoCDP = "CREATE TABLE IF NOT EXISTS INFO_CDP ("
                                        "ID_CDP INT PRIMARY KEY NOT NULL, "
                                        "ID_CLIENTE INT NOT NULL, "
                                        "MONTO REAL NOT NULL, "
                                        "INTERES REAL NOT NULL, "
                                        "FECHA_INICIAL TEXT NOT NULL, "
                                        "FECHA_VENCIMIENTO TEXT NOT NULL, "
                                        "FOREIGN KEY(ID_CLIENTE) REFERENCES INFOCLIENTES(IDCLIENTE));";
    rc = sqlite3_exec(db, tablaInfoCDP, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Tabla INFO_CDP creada con éxito" << endl;
    }

    const char *tablaPagoPrestamos = "CREATE TABLE IF NOT EXISTS PAGO_PRESTAMOS ("
                                              "ID_PAGO INT PRIMARY KEY NOT NULL, "
                                              "ID_PRESTAMO INT NOT NULL, "
                                              "MONTO REAL NOT NULL, "
                                              "PAGO_CAPITAL REAL NOT NULL, "
                                              "PAGO_INTERES REAL NOT NULL, "
                                              "FECHA_PAGO TEXT NOT NULL, "
                                              "FOREIGN KEY(ID_PRESTAMO) REFERENCES INFO_PRESTAMOS(ID_PRESTAMO));";
    rc = sqlite3_exec(db, tablaPagoPrestamos, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Tabla PAGO_PRESTAMOS creada con éxito" << endl;
    }

    const char *tablaTransacciones = "CREATE TABLE IF NOT EXISTS TRANSACCIONES ("
                                              "ID_TRANSACCION INT PRIMARY KEY NOT NULL, "
                                              "ID_CUENTA INT NOT NULL, "
                                              "ID_CUENTA_DESTINO INT, "
                                              "TIPO_TRANSACCION TEXT NOT NULL, "
                                              "MONTO REAL NOT NULL, "
                                              "FECHA_TRANSACCION TEXT NOT NULL, "
                                              "FOREIGN KEY(ID_CUENTA) REFERENCES CUENTAS(ID_CUENTA));";
    rc = sqlite3_exec(db, tablaTransacciones, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Tabla TRANSACCIONES creada con éxito" << endl;
    }

    // Menú inicial
    int opcion = 0;
    int idCliente;
    int opcionCliente = 0;

    while (opcion != 3) {
        menuInicial();
        cin >> opcion;
        switch (opcion) {
            case 1:
                cout << "Modo de atencion a clientes seleccionado" << endl;
                opcionCliente = 0;
                while (opcionCliente != 10) {
                    menuAtencionClientes();
                    cin >> opcionCliente;
                    switch (opcionCliente) {
                        case 1:
                            verCuentas(db);
                            break;
                        case 2:
                            realizarDeposito(db);
                            break;
                        case 3:
                            realizarRetiro(db);
                            break;
                        case 4:
                            realizarTransferencia(db);
                            break;
                        case 5:{
                            int opcionPrestamos = 0;
                            while (opcionPrestamos != 5) {
                                menuPrestamos();
                                cin >> opcionPrestamos;
                                switch (opcionPrestamos) {
                                    case 1:
                                        solicitarPrestamo(db);
                                        break;
                                    case 2:
                                        verPrestamos(db);
                                        break;
                                    case 3:
                                        generarReportePrestamos(db);
                                        break;
                                    case 4:
                                        pagarPrestamo(db);
                                        break;
                                    case 5:
                                        cout << "Saliendo del menu de prestamos..." << endl;
                                        break;
                                    default:
                                        cout << "Opcion no valida. Por favor, intente de nuevo." << endl;
                                }
                            }
                            break;
                        }
                        case 6:
                        {
                            int opcionCDP = 0;
                            while (opcionCDP != 4) 
                            {
                                menuCDP();
                                cin >> opcionCDP;
                                switch (opcionCDP) 
                                {
                                    case 1:
                                        crearCDP(db);
                                        break;
                                    case 2:
                                        verCDP(db);
                                        break;
                                    case 3:
                                        eliminarCDP(db);
                                        break;
                                    case 4:
                                        cout << "Saliendo del menu de CDP..." << endl;
                                        break;
                                    default:
                                    cout << "Opcion no valida. Por favor, intente de nuevo." << endl;
                                }
                            }
                            break;
                        }

                        case 7:
                            verTransacciones(db);
                            break;
                        case 8:
                            agregarCliente(db);
                            break;
                        case 9:
                            crearCuenta(db);
                            break;
                        case 10:
                            cout << "Saliendo del menu de atencion a clientes..." << endl;
                            break;
                        default:
                            cout << "Opcion no valida. Por favor, intente de nuevo." << endl;
                    }
                    
                }
                break;
            case 2:{
                cout << "Modo de informacion general seleccionado" << endl;
                int opcionInformacion = 0;
                while (opcionInformacion != 4) {
                    menuInformacionPrestamos();
                    cin >> opcionInformacion;
                    switch (opcionInformacion) {
                        case 1:
                            verInformacionPrestamos(db);
                            break;
                        case 2:
                            calcularTablaPagos(db);
                            break;
                        case 3:
                            agregarTipoPrestamo(db);
                            break;
                        case 4:
                            cout << "Saliendo del menu de informacion general..." << endl;
                            break;
                        default:
                            cout << "Opcion no valida. Por favor, intente de nuevo." << endl;
                    }
                }
                break;
            }
            case 3:
                cout << "Saliendo del sistema..." << endl;
                break;
            default:
                cout << "Opcion no valida. Por favor, intente de nuevo." << endl;
        }
    }

    sqlite3_close(db);
    return 0;
}
