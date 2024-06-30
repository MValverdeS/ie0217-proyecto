/**
 * @file  prestamos.cpp
 * @brief Se definen las funciones asociadas con prestamos
 *
 * @author Gabriel González Rivera B93432
 * @author Edgar Marcelo Valverde Solís C08089
 * @author Daniel Rodríguez Rivas B96719
 * @date 30/6/2024
 * 
 * Licencia: MIT
 */



#include <iostream>
#include <cmath>
#include <sqlite3.h>
#include <fstream>
#include "prestamos.hpp"
#include "utilidades.hpp"

using namespace std;

/**
 * @brief Función para solicitar y procesar la solicitud de un préstamo.
 *
 * @param db Puntero a la base de datos SQLite.
 */
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
    double tasaInteres1 = tasaInteres;
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
    sqlite3_bind_double(stmt, 7, tasaInteres1);
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

/**
 * @brief La función verInformacionPrestamos muestra en pantalla la información detallada de todos los tipos de préstamos 
 * disponibles en la base de datos. Cada tipo de préstamo se muestra con su ID único, nombre del tipo, tasa de interés 
 * aplicable en porcentaje y plazo en meses.Se ejecuta una consulta SQL para seleccionar todos los registros de la tabla 
 * TIPOS_PRESTAMO. Para cada registro obtenido, se extraen y muestran los siguientes datos: ID del tipo de préstamo,
 * Nombre del tipo de préstamo, Tasa de interés en formato porcentaje (%) y Plazo del préstamo en meses. Si la preparación de 
 * la consulta SQL no es exitosa, se muestra un mensaje de error indicando el motivo y se termina la función. Cada tipo de préstamo 
 * se muestra en una línea con un formato legible que incluye todos los detalles mencionados.
 * 
 * @param db Puntero a la base de datos sqlite3 donde se realizará la consulta para obtener la información de los préstamos.
 */
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

/**
 * @brief La función calcularTablaPagos permite al usuario calcular y visualizar la tabla de pagos
 * para un préstamo personalizado o seleccionando uno de los tipos de préstamo existentes en la base de datos.
 * 
 * El usuario elige entre dos opciones de cálculo:
 * 1. Calcular de manera personalizada: El usuario ingresa el monto del préstamo, la tasa de interés mensual
 *    y el plazo en meses.
 * 2. Calcular con tipos de préstamo existentes: Se muestran los tipos de préstamo disponibles desde la base de datos,
 *    el usuario selecciona uno por su ID y luego ingresa el monto del préstamo.
 * 
 * Para la opción 1:
 * - Se solicitan y validan el monto del préstamo, la tasa de interés (en porcentaje anual) y el plazo en meses.
 * - Se calcula la cuota mensual y se muestra en pantalla.
 * - Se genera y muestra una tabla detallada de pagos mensuales que incluye el número de mes, el pago mensual,
 *   el interés, el principal pagado y el saldo restante del préstamo.
 * 
 * Para la opción 2:
 * - Se muestran los tipos de préstamo disponibles con su ID, tipo, tasa de interés y plazo en meses.
 * - El usuario selecciona un tipo de préstamo por su ID.
 * - Se solicita y valida el monto del préstamo.
 * - Se calcula la cuota mensual y se muestra en pantalla.
 * - Se genera y muestra una tabla detallada de pagos mensuales similar a la opción 1.
 * 
 * Si la preparación de consultas SQL no tiene éxito en la obtención de tipos de préstamo o detalles específicos,
 * se muestra un mensaje de error correspondiente.
 * 
 * La función utiliza funciones auxiliares para convertir la tasa de interés anual a mensual y calcular la cuota mensual.
 * 
 * @param db Puntero a la base de datos sqlite3 donde se realizarán las consultas para obtener tipos de préstamo.
 */

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

/**
 * @brief Permite al cliente realizar el pago de un préstamo.
 * 
 * Esta función permite al usuario ingresar el ID del cliente y el ID del préstamo que desea pagar.
 * Verifica la existencia del cliente y obtiene la información del préstamo.
 * Permite al usuario seleccionar entre pagar un monto personalizado o una cantidad de cuotas.
 * Realiza los cálculos necesarios para verificar y procesar el pago, actualizando el saldo de la cuenta
 * y registrando el pago en la base de datos.
 * 
 * @param db Puntero a la base de datos SQLite.
 */
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
    cout << "Ingrese el ID del prestamo que desea pagar: ";
    cin >> idPrestamo;

    // Obtener información del préstamo
    const char *sqlInfoPrestamo = "SELECT MONTO, TASA_INTERES, PLAZO, MONEDA, CUOTA_MENSUAL FROM INFO_PRESTAMOS WHERE ID_PRESTAMO = ? AND ID_CLIENTE = ?";
    sqlite3_stmt *stmtInfo;
    if (sqlite3_prepare_v2(db, sqlInfoPrestamo, -1, &stmtInfo, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta de información del préstamo: " << sqlite3_errmsg(db) << endl;
        return;
    }
    sqlite3_bind_int(stmtInfo, 1, idPrestamo);
    sqlite3_bind_int(stmtInfo, 2, idCliente);

    double montoRestante, tasaInteres, cuotaMensual;
    int plazo;
    string monedaPrestamo;
    if (sqlite3_step(stmtInfo) == SQLITE_ROW) {
        montoRestante = sqlite3_column_double(stmtInfo, 0);
        tasaInteres = sqlite3_column_double(stmtInfo, 1);
        plazo = sqlite3_column_int(stmtInfo, 2);
        monedaPrestamo = reinterpret_cast<const char*>(sqlite3_column_text(stmtInfo, 3));
        cuotaMensual = sqlite3_column_double(stmtInfo, 4);
        cout << "El préstamo está en " << monedaPrestamo << " con una tasa de interés de " << tasaInteres << "% y un monto restante de " << montoRestante << "." << endl;
    } else {
        cerr << "Préstamo no encontrado." << endl;
        sqlite3_finalize(stmtInfo);
        return;
    }
    sqlite3_finalize(stmtInfo);

    // Calcular cantidad de cuotas pagadas
    const char *sqlSumaPagos = "SELECT SUM(MONTO) FROM PAGO_PRESTAMOS WHERE ID_PRESTAMO = ?";
    sqlite3_stmt *stmtSumaPagos;

    if (sqlite3_prepare_v2(db, sqlSumaPagos, -1, &stmtSumaPagos, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta de suma de pagos: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmtSumaPagos, 1, idPrestamo);

    double sumaPagos = 0.0;
    if (sqlite3_step(stmtSumaPagos) == SQLITE_ROW) {
        sumaPagos = sqlite3_column_double(stmtSumaPagos, 0);
    }
    sqlite3_finalize(stmtSumaPagos);

    double cuotasPagadas = sumaPagos / cuotaMensual;
    cout << "Cantidad de cuotas pagadas: " << round(cuotasPagadas * 100) / 100 << endl;

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

    // Elegir si pagar monto personalizado o cantidad de cuotas
    int opcionPago;
    cout << "Seleccione el tipo de pago:\n1. Monto personalizado\n2. Cantidad de cuotas" << endl;
    cin >> opcionPago;

    double montoPago = 0.0;
    double montoPagoOriginal = 0.0;

    if (opcionPago == 1) {
        while (true) {
            cout << "Ingrese el monto a pagar en " << monedaCuenta << ": ";
            cin >> montoPago;
            if (cin.fail()) {
                cin.clear(); // Limpiar el estado de error de cin
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignorar la entrada incorrecta
                cout << "Entrada inválida. Por favor, introduce un número.\n";
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

    } else if (opcionPago == 2) {
        int cantidadCuotas;
        cout << "Ingrese la cantidad de cuotas que desea pagar: ";
        cin >> cantidadCuotas;

        montoPago = cuotaMensual * cantidadCuotas;

        if (monedaPrestamo != monedaCuenta) {
            montoPagoOriginal = convertirMoneda(montoPago, monedaPrestamo, monedaCuenta);
        } else {
            montoPagoOriginal = montoPago;
        }

        if (saldoCuenta < montoPagoOriginal) {
            cerr << "Saldo insuficiente en la cuenta." << endl;
            return;
        }
    }

    // Verificar que el monto a pagar es suficiente para la cuota
    double pagoInteres = montoRestante * (tasaInteres / 100 / 12);
    double pagoCapital = montoPago - pagoInteres;

    if (pagoCapital <= 0) {
        cerr << "El monto ingresado no es suficiente para cubrir los intereses del préstamo." << endl;
        return;
    }

    double montoSobrante = 0.0;
    double montoDevolver = 0.0;
    if (pagoCapital > montoRestante) {
        montoSobrante = pagoCapital - montoRestante;
        montoDevolver = convertirMoneda(montoSobrante, monedaPrestamo, monedaCuenta);
        pagoCapital = montoRestante;
        montoRestante = 0.0;
    } else {
        montoRestante -= pagoCapital;
    }

    // Actualizar el saldo de la cuenta
    double montoRetirar = montoPagoOriginal - montoDevolver;
    const char *sqlActualizarCuenta = "UPDATE CUENTAS SET MONTO = MONTO - ? WHERE ID_CUENTA = ? AND ID_CLIENTE = ?";
    sqlite3_stmt *stmtActualizarCuenta;

    if (sqlite3_prepare_v2(db, sqlActualizarCuenta, -1, &stmtActualizarCuenta, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta de actualización de la cuenta: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_double(stmtActualizarCuenta, 1, montoRetirar);
    sqlite3_bind_int(stmtActualizarCuenta, 2, idCuenta);
    sqlite3_bind_int(stmtActualizarCuenta, 3, idCliente);

    if (sqlite3_step(stmtActualizarCuenta) != SQLITE_DONE) {
        cerr << "No se pudo actualizar el saldo de la cuenta: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmtActualizarCuenta);
        return;
    }
    sqlite3_finalize(stmtActualizarCuenta);

    // Actualizar el monto del préstamo
    const char *sqlActualizarPrestamo;
    if (montoRestante == 0.0) {
        sqlActualizarPrestamo = "UPDATE INFO_PRESTAMOS SET MONTO = 0 WHERE ID_PRESTAMO = ? AND ID_CLIENTE = ?";
    } else {
        sqlActualizarPrestamo = "UPDATE INFO_PRESTAMOS SET MONTO = ? WHERE ID_PRESTAMO = ? AND ID_CLIENTE = ?";
    }

    sqlite3_stmt *stmtActualizar;
    if (sqlite3_prepare_v2(db, sqlActualizarPrestamo, -1, &stmtActualizar, 0) != SQLITE_OK) {
        cerr << "No se pudo preparar la consulta de actualización del préstamo: " << sqlite3_errmsg(db) << endl;
        return;
    }

    if (montoRestante == 0.0) {
        sqlite3_bind_int(stmtActualizar, 1, idPrestamo);
        sqlite3_bind_int(stmtActualizar, 2, idCliente);
    } else {
        sqlite3_bind_double(stmtActualizar, 1, montoRestante);
        sqlite3_bind_int(stmtActualizar, 2, idPrestamo);
        sqlite3_bind_int(stmtActualizar, 3, idCliente);
    }

    if (sqlite3_step(stmtActualizar) != SQLITE_DONE) {
        cerr << "No se pudo actualizar el préstamo: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmtActualizar);
        return;
    }
    sqlite3_finalize(stmtActualizar);

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

    if (montoSobrante > 0) {
        cout << "Sobraron " << montoSobrante << " " << monedaPrestamo << ", se devolverán " << montoDevolver << " " << monedaCuenta << " a su cuenta." << endl;
    }

    if (montoRestante == 0.0) {
        cout << "El préstamo completo ya fue pagado." << endl;
    }

    sqlite3_finalize(stmtPago);
}

/**
 * @brief Muestra los préstamos de un cliente específico.
 * 
 * Esta función solicita al usuario ingresar el ID del cliente cuyos préstamos desea visualizar.
 * Realiza una consulta a la base de datos para obtener los detalles de los préstamos del cliente.
 * Imprime los detalles de cada préstamo en la consola.
 * 
 * @param db Puntero a la base de datos SQLite.
 */
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

/**
 * @brief Genera un reporte de los préstamos de un cliente y lo guarda en un archivo de texto.
 * 
 * Esta función solicita al usuario ingresar el ID del cliente cuyos préstamos desea reportar.
 * Realiza una consulta a la base de datos para obtener los detalles de los préstamos del cliente.
 * Escribe los detalles de cada préstamo en un archivo de texto llamado 'reporte_prestamos.txt'.
 * 
 * @param db Puntero a la base de datos SQLite.
 */

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

/**
 * @brief Agrega un nuevo tipo de préstamo a la base de datos.
 * 
 * Esta función permite al usuario ingresar los detalles de un nuevo tipo de préstamo y lo agrega a la tabla `TIPOS_PRESTAMO` en la base de datos.
 * 
 * @param db Puntero a la base de datos SQLite.
 */
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

/**
 * @brief La función imprimirTiposPrestamo obtiene y muestra todos los tipos de préstamo disponibles
 * almacenados en la base de datos SQLite especificada. La función realiza una consulta SQL para seleccionar 
 * todos los registros de la tabla TIPOS_PRESTAMO. Luego, recorre los resultados obtenidos y muestra en la consola cada tipo de préstamo con su ID, tipo,
 * tasa de interés mensual y plazo en meses. Si la preparación de la consulta SQL no tiene éxito, se imprime un mensaje de error indicando la razón del fallo.
 * 
 * @param db Puntero a la base de datos sqlite3 donde se realizará la consulta para obtener tipos de préstamo.
 */
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