#include <iostream>
#include <sqlite3.h>
#include "cuentas.hpp"
#include "utilidades.hpp"

#include <cmath>
#include <fstream>

using namespace std;

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
