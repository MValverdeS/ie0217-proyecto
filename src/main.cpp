#include <iostream>
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
    while (opcion != 3) {
        menuInicial();
        cin >> opcion;
        switch (opcion) {
            case 1:
                cout << "Modo de atencion a clientes seleccionado" << endl;
                // Llamar a la función correspondiente para atención a clientes
                break;
            case 2:
                cout << "Modo de informacion general seleccionado" << endl;
                // Llamar a la función correspondiente para información general
                break;
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