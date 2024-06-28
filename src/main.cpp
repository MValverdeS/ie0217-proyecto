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
    double monto;
    cout << "Ingrese el ID de la cuenta en la que desea realizar el deposito: ";
    cin >> idCuenta;
    cout << "Ingrese el monto a depositar: ";
    cin >> monto;

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
        registrarTransaccion(db, idCuenta, "Deposito", monto);
    }

    sqlite3_finalize(stmt);
}

void crearCDP(sqlite3* db) {
    int idCliente;
    cout << "Ingrese el ID del Cliente: ";
    cin >> idCliente;

    int idCDP;
    double monto, interes;
    string fechaInicial, fechaVencimiento;

    cout << "Ingrese el ID del CDP: ";
    cin >> idCDP;
    cout << "Ingrese el monto del CDP: ";
    cin >> monto;
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

    sqlite3_bind_int(stmt, 1, idCDP);
    sqlite3_bind_int(stmt, 2, idCliente);
    sqlite3_bind_double(stmt, 3, monto);
    sqlite3_bind_double(stmt, 4, interes);
    sqlite3_bind_text(stmt, 5, fechaInicial.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, fechaVencimiento.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cerr << "No se pudo crear el CDP: " << sqlite3_errmsg(db) << endl;
    } else {
        cout << "CDP creado con exito." << endl;
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
    cout << "Ingrese el monto a retirar: ";
    cin >> monto;

    // Verificar si la cuenta tiene suficiente saldo
    const char *verificarSaldoSql = "SELECT MONTO FROM CUENTAS WHERE ID_CUENTA = ? AND ID_CLIENTE = ?";
    sqlite3_stmt *stmtSaldo;
    if (sqlite3_prepare_v2(db, verificarSaldoSql, -1, &stmtSaldo, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmtSaldo, 1, idCuenta);
    sqlite3_bind_int(stmtSaldo, 2, idCliente);

    if (sqlite3_step(stmtSaldo) == SQLITE_ROW) {
        double saldoActual = sqlite3_column_double(stmtSaldo, 0);
        if (saldoActual < monto) {
            cerr << "Saldo insuficiente." << endl;
            sqlite3_finalize(stmtSaldo);
            return;
        }
    } else {
        cerr << "Cuenta no encontrada." << endl;
        sqlite3_finalize(stmtSaldo);
        return;
    }

    sqlite3_finalize(stmtSaldo);

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
        cout << "Retiro realizado con exito." << endl;
        registrarTransaccion(db, idCuenta, "Retiro", monto);
    }

    sqlite3_finalize(stmt);
}

void realizarTransferencia(sqlite3* db) {
    int idCliente;
    cout << "Ingrese el ID del Cliente: ";
    cin >> idCliente;

    int idCuentaOrigen, idCuentaDestino;
    double monto;
    cout << "Ingrese el ID de la cuenta desde la que desea transferir: ";
    cin >> idCuentaOrigen;
    cout << "Ingrese el ID de la cuenta de destino: ";
    cin >> idCuentaDestino;
    cout << "Ingrese el monto a transferir: ";
    cin >> monto;

    // Verificar si la cuenta de origen tiene suficiente saldo
    const char *verificarSaldoSql = "SELECT MONTO FROM CUENTAS WHERE ID_CUENTA = ? AND ID_CLIENTE = ?";
    sqlite3_stmt *stmtSaldo;
    if (sqlite3_prepare_v2(db, verificarSaldoSql, -1, &stmtSaldo, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmtSaldo, 1, idCuentaOrigen);
    sqlite3_bind_int(stmtSaldo, 2, idCliente);

    if (sqlite3_step(stmtSaldo) == SQLITE_ROW) {
        double saldoActual = sqlite3_column_double(stmtSaldo, 0);
        if (saldoActual < monto) {
            cerr << "Saldo insuficiente." << endl;
            sqlite3_finalize(stmtSaldo);
            return;
        }
    } else {
        cerr << "Cuenta no encontrada." << endl;
        sqlite3_finalize(stmtSaldo);
        return;
    }

    sqlite3_finalize(stmtSaldo);

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

    sqlite3_bind_double(stmtDestino, 1, monto);
    sqlite3_bind_int(stmtDestino, 2, idCuentaDestino);

    if (sqlite3_step(stmtDestino) != SQLITE_DONE) {
        cerr << "No se pudo realizar la transferencia: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmtDestino);
        sqlite3_exec(db, "ROLLBACK;", 0, 0, 0);
        return;
    }

    sqlite3_finalize(stmtDestino);
    sqlite3_exec(db, "COMMIT;", 0, 0, 0);

    cout << "Transferencia realizada con exito." << endl;
    registrarTransaccion(db, idCuentaOrigen, "Transferencia", monto, idCuentaDestino);
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

void calcularTablaPagos() {
    double monto, tasaInteres;
    int plazoMeses;

    cout << "Ingrese el monto del prestamo: ";
    cin >> monto;
    cout << "Ingrese la tasa de interes (en %): ";
    cin >> tasaInteres;
    cout << "Ingrese el plazo en meses: ";
    cin >> plazoMeses;

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
}

void solicitarPrestamo(sqlite3* db) {
    int idCliente;
    cout << "Ingrese el ID del Cliente: ";
    cin >> idCliente;

    int idPrestamo, plazo;
    double monto, tasaInteres, cuotaMensual;
    string tipoPrestamo, moneda;

    cout << "Ingrese el ID del prestamo: ";
    cin >> idPrestamo;
    cout << "Ingrese el tipo de prestamo: ";
    cin >> tipoPrestamo;
    cout << "Ingrese la moneda del prestamo: ";
    cin >> moneda;
    cout << "Ingrese el monto del prestamo: ";
    cin >> monto;
    cout << "Ingrese la tasa de interes (en %): ";
    cin >> tasaInteres;
    cout << "Ingrese el plazo en meses: ";
    cin >> plazo;

    tasaInteres = tasaInteres / 100.0;
    cuotaMensual = (monto * tasaInteres) / (1 - pow(1 + tasaInteres, -plazo));

    const char *sql = "INSERT INTO INFO_PRESTAMOS (ID_PRESTAMO, ID_CLIENTE, TIPO_PRESTAMO, MONEDA, MONTO, TASA_INTERES, PLAZO, CUOTA_MENSUAL) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, idPrestamo);
    sqlite3_bind_int(stmt, 2, idCliente);
    sqlite3_bind_text(stmt, 3, tipoPrestamo.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, moneda.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 5, monto);
    sqlite3_bind_double(stmt, 6, tasaInteres);
    sqlite3_bind_int(stmt, 7, plazo);
    sqlite3_bind_double(stmt, 8, cuotaMensual);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cerr << "No se pudo insertar el prestamo: " << sqlite3_errmsg(db) << endl;
    } else {
        cout << "Prestamo solicitado con exito." << endl;
    }

    sqlite3_finalize(stmt);
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

    int idPrestamo;
    double montoPago, pagoCapital, pagoInteres;

    cout << "Ingrese el ID del prestamo que desea pagar: ";
    cin >> idPrestamo;
    cout << "Ingrese el monto a pagar: ";
    cin >> montoPago;

    // Obtener información del préstamo
    const char *sqlInfoPrestamo = "SELECT MONTO, TASA_INTERES, PLAZO FROM INFO_PRESTAMOS WHERE ID_PRESTAMO = ? AND ID_CLIENTE = ?";
    sqlite3_stmt *stmtInfo;
    if (sqlite3_prepare_v2(db, sqlInfoPrestamo, -1, &stmtInfo, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta de información del préstamo: " << sqlite3_errmsg(db) << endl;
        return;
    }
    sqlite3_bind_int(stmtInfo, 1, idPrestamo);
    sqlite3_bind_int(stmtInfo, 2, idCliente);

    double montoRestante, tasaInteres;
    int plazo;
    if (sqlite3_step(stmtInfo) == SQLITE_ROW) {
        montoRestante = sqlite3_column_double(stmtInfo, 0);
        tasaInteres = sqlite3_column_double(stmtInfo, 1);
        plazo = sqlite3_column_int(stmtInfo, 2);
    } else {
        cerr << "Préstamo no encontrado." << endl;
        sqlite3_finalize(stmtInfo);
        return;
    }
    sqlite3_finalize(stmtInfo);

    // Calcular los pagos de capital e interés
    pagoInteres = montoRestante * (tasaInteres / 100 / 12);
    pagoCapital = montoPago - pagoInteres;
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
    }
    sqlite3_finalize(stmtActualizar);

    // Registrar el pago en la tabla de pagos
    const char *sqlRegistrarPago = "INSERT INTO PAGO_PRESTAMOS (ID_PRESTAMO, MONTO, PAGO_CAPITAL, PAGO_INTERES, FECHA_PAGO) "
                                   "VALUES (?, ?, ?, ?, datetime('now'))";
    sqlite3_stmt *stmtPago;
    if (sqlite3_prepare_v2(db, sqlRegistrarPago, -1, &stmtPago, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta de registro de pago: " << sqlite3_errmsg(db) << endl;
        return;
    }
    sqlite3_bind_int(stmtPago, 1, idPrestamo);
    sqlite3_bind_double(stmtPago, 2, montoPago);
    sqlite3_bind_double(stmtPago, 3, pagoCapital);
    sqlite3_bind_double(stmtPago, 4, pagoInteres);

    if (sqlite3_step(stmtPago) != SQLITE_DONE) {
        cerr << "No se pudo registrar el pago: " << sqlite3_errmsg(db) << endl;
    } else {
        cout << "Pago del préstamo realizado con éxito." << endl;
        registrarTransaccion(db, idCliente, "Pago de Prestamo", montoPago, idPrestamo);
    }

    sqlite3_finalize(stmtPago);
}

void agregarCliente(sqlite3* db) {
    int idCliente, cedula;
    string nombre, apellido;

    cout << "Ingrese el ID del Cliente: ";
    cin >> idCliente;
    cout << "Ingrese la Cedula del Cliente: ";
    cin >> cedula;
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
        cout << "Cliente agregado con exito." << endl;
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
    cout << "Ingrese el tipo de cuenta (Dolares/Colones): ";
    cin >> tipoCuenta;

    if (tipoCuenta != "Dolares" && tipoCuenta != "Colones") {
        cerr << "Tipo de cuenta no válido." << endl;
        return;
    }

    // Solicitar monto inicial
    double montoInicial;
    cout << "Ingrese el monto inicial para la cuenta en " << tipoCuenta << ": ";
    cin >> montoInicial;

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
                            calcularTablaPagos();
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
