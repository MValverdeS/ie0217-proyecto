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

// Se crea un menu que da las opciones para atencion al cliente

void menuAtencionClientes() {
    cout << "Menu de Atencion a Clientes" << endl;
    cout << "1. Ver Cuentas" << endl;
    cout << "2. Realizar Deposito" << endl;
    cout << "3. Realizar Retiro" << endl;
    cout << "4. Realizar Transferencia" << endl;
    cout << "5. Prestamos" << endl;
    cout << "6. CDP" << endl;
    cout << "7. Ver Transacciones" << endl;
    cout << "8. Salir" << endl;
}

void menuInformacionPrestamos() {
    cout << "Menu de Informacion General sobre Prestamos" << endl;
    cout << "1. Ver Informacion de Prestamos" << endl;
    cout << "2. Calcular Tabla de Pagos" << endl;
    cout << "3. Salir" << endl;
}

void menuPrestamos() {
    cout << "Menu de Prestamos" << endl;
    cout << "1. Solicitar Prestamo" << endl;
    cout << "2. Ver Prestamos" << endl;
    cout << "3. Generar Reporte de Prestamos" << endl;
    cout << "4. Salir" << endl;
}

void menuCDP() {
    cout << "Menu de CDP" << endl;
    cout << "1. Crear CDP" << endl;
    cout << "2. Ver CDP" << endl;
    cout << "3. Eliminar CDP" << endl;
    cout << "4. Salir" << endl;

}

void verCuentas(sqlite3* db, int idCliente) {
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

void verTransacciones(sqlite3* db, int idCliente) {
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

void realizarDeposito(sqlite3* db, int idCliente) {
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

void crearCDP(sqlite3* db, int idCliente) {
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


void verCDP(sqlite3* db, int idCliente) {
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

void eliminarCDP(sqlite3* db, int idCliente) {
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

void realizarRetiro(sqlite3* db, int idCliente) {
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

void realizarTransferencia(sqlite3* db, int idCliente) {
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

void solicitarPrestamo(sqlite3* db, int idCliente) {
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

void verPrestamos(sqlite3* db, int idCliente) {
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


void generarReportePrestamos(sqlite3* db, int idCliente) {
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
                cout << "Ingrese su ID de Cliente: ";
                cin >> idCliente;
                while (opcionCliente != 8) {
                    menuAtencionClientes();
                    cin >> opcionCliente;
                    switch (opcionCliente) {
                        case 1:
                            verCuentas(db, idCliente);
                            break;
                        case 2:
                            realizarDeposito(db, idCliente);
                            break;
                        case 3:
                            realizarRetiro(db, idCliente);
                            break;
                        case 4:
                            realizarTransferencia(db, idCliente);
                            break;
                        case 5:{
                            int opcionPrestamos = 0;
                            while (opcionPrestamos != 4) {
                                menuPrestamos();
                                cin >> opcionPrestamos;
                                switch (opcionPrestamos) {
                                    case 1:
                                        solicitarPrestamo(db, idCliente);
                                        break;
                                    case 2:
                                        verPrestamos(db, idCliente);
                                        break;
                                    case 3:
                                        generarReportePrestamos(db, idCliente);
                                        break;
                                    case 4:
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
                                        crearCDP(db, idCliente);
                                        break;
                                    case 2:
                                        verCDP(db, idCliente);
                                        break;
                                    case 3:
                                        eliminarCDP(db, idCliente);
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
                            verTransacciones(db, idCliente);
                            break;
                        case 8:
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
                while (opcionInformacion != 3) {
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
