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

    const char *sql = "PRAGMA foreign_keys = ON;";

    

    /*Primera tabla (información cliente) */

    // Crea una tabla INFOCLIENTES
    sql = "CREATE TABLE IF NOT EXISTS INFOCLIENTES("
                      "IDCLIENTE INT PRIMARY KEY NOT NULL, "
                      "CEDULA INT NOT NULL, "
                      "NOMBRE TEXT NOT NULL, "
                      "APELLIDO TEXT NOT NULL); ";

    rc = sqlite3_exec(db, sql, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Tabla creada con éxito" << endl;
    }

    // Inserta datos en la tabla
    sql = "INSERT INTO INFOCLIENTES (IDCLIENTE, CEDULA, NOMBRE, APELLIDO) "
        "VALUES (1, 117990097, 'Gabriel', 'Gonza'); ";

    rc = sqlite3_exec(db, sql, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Records created successfully" << endl;
    }

    // Selecciona datos de la tabla
    sql = "SELECT * from INFOCLIENTES";
    rc = sqlite3_exec(db, sql, callback, (void*)data, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Operation done successfully" << endl;
    }




    /*Segunda tabla (Tipos de préstamos)*/

    
    const char *sql1 = "CREATE TABLE IF NOT EXISTS TIPOS_PRESTAMO("
                   "ID_TIPO INT PRIMARY KEY NOT NULL, "
                   "TIPO TEXT NOT NULL, "
                   "TAZA_INTERES REAL NOT NULL, " 
                   "PLAZO_MESES INT NOT NULL);";

    
    rc = sqlite3_exec(db, sql1, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Tabla creada con éxito" << endl;
    }




    // Inserta datos en la tabla
    sql1 = "INSERT INTO TIPOS_PRESTAMO(ID_TIPO, TIPO, TAZA_INTERES, PLAZO_MESES) "
        "VALUES (1, 'Prendario' , 5.33, 6); ";

    rc = sqlite3_exec(db, sql1, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Records created successfully" << endl;
    }

    // Selecciona datos de la tabla
    sql1= "SELECT * from TIPOS_PRESTAMO ";
    rc = sqlite3_exec(db, sql1, callback, (void*)data, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Operation done successfully" << endl;
    }
    



    /*Tercera tabla (Registro cuentas)*/
    const char *sql2 = "CREATE TABLE IF NOT EXISTS CUENTAS("
                   "ID_CUENTA INT PRIMARY KEY NOT NULL, "
                   "ID_CLIENTE INT NOT NULL, "
                   "TIPO TEXT NOT NULL,"
                   "MONTO REAL NOT NULL, " 
                   "FOREIGN KEY(ID_CLIENTE) REFERENCES INFOCLIENTES(IDCLIENTE));";


     rc = sqlite3_exec(db, sql2, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Tabla creada con éxito" << endl;
    }


      // Inserta datos en la tabla
    sql2 = "INSERT INTO CUENTAS (ID_CUENTA, ID_CLIENTE, TIPO, MONTO) "
        "VALUES (3455, 1, 'Colones', 3252.32); ";

    rc = sqlite3_exec(db, sql2, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Records created successfully" << endl;
    }

    // Selecciona datos de la tabla
    sql2= "SELECT * from CUENTAS";
    rc = sqlite3_exec(db, sql2, callback, (void*)data, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Operation done successfully" << endl;
    }



    /*Cuarta tabla (Información préstamos)*/

    
    const char *sql3 = "CREATE TABLE IF NOT EXISTS INFO_PRESTAMOS("
                   "ID_PRESTAMO INT PRIMARY KEY NOT NULL, "
                   "ID_CLIENTE INT NOT NULL, "
                   "TIPO_ID INT NOT NULL,"
                   "TIPO_PRESTAMO TEXT NOT NULL, " 
                   "MONEDA TEXT NOT NULL, " 
                   "MONTO REAL NOT NULL, " 
                   "TAZA_INTERES REAL NOT NULL, "
                   "PLAZO INT NOT NULL, " 
                   "CUOTA_MENSUAL REAL NOT NULL, "  
                   "FOREIGN KEY(ID_CLIENTE) REFERENCES INFOCLIENTES(IDCLIENTE)"
                   "FOREIGN KEY(TIPO_ID) REFERENCES TIPOS_PRESTAMO(ID_TIPO));";

    
    rc = sqlite3_exec(db, sql3, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Tabla creada con éxito" << endl;
    }

    
    // Inserta datos en la tabla
    sql3 = "INSERT INTO INFO_PRESTAMOS (ID_PRESTAMO, ID_CLIENTE, TIPO_ID, TIPO_PRESTAMO, MONEDA, MONTO, TAZA_INTERES, PLAZO, CUOTA_MENSUAL) "
        "VALUES (22, 1, 1, 'Prendario', 'Colones', 30000, 5.33, 4, 230000); ";

    rc = sqlite3_exec(db, sql3, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Records created successfully" << endl;
    }

    // Selecciona datos de la tabla
    sql3= "SELECT * from INFO_PRESTAMOS";
    rc = sqlite3_exec(db, sql3, callback, (void*)data, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Operation done successfully" << endl;
    }


 /*Quinta tabla (Información certificados a plazo)*/

    
    const char *sql4 = "CREATE TABLE IF NOT EXISTS INFO_CDP("
                   "ID_CDP INT PRIMARY KEY NOT NULL, "
                   "ID_CLIENTE INT NOT NULL, "
                   "MONTO REAL NOT NULL, " 
                   "INTERES REAL NOT NULL, "
                   "FECHA_INICIAL TEXT NOT NULL, " 
                   "FECHA_VENCIMIENTO TEXT NOT NULL, "  
                   "FOREIGN KEY(ID_CLIENTE) REFERENCES INFOCLIENTES(IDCLIENTE));";

    
    rc = sqlite3_exec(db, sql4, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Tabla creada con éxito" << endl;
    }

    
    // Inserta datos en la tabla
    sql4 = "INSERT INTO INFO_CDP (ID_CDP, ID_CLIENTE, MONTO,INTERES, FECHA_INICIAL, FECHA_VENCIMIENTO) "
        "VALUES (4, 1, 400000, 2, '2/6/2023', '2/6/2024'); ";

    rc = sqlite3_exec(db, sql4, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Records created successfully" << endl;
    }

    // Selecciona datos de la tabla
    sql4= "SELECT * from INFO_CDP";
    rc = sqlite3_exec(db, sql4, callback, (void*)data, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Operation done successfully" << endl;
    }


    /*Sexta tabla (Registro pago préstamos)*/

    
    const char *sql5 = "CREATE TABLE IF NOT EXISTS PAGO_PRESTAMOS("
                   "ID_PAGO INT PRIMARY KEY NOT NULL, "
                   "ID_PRESTAMO INT NOT NULL, "
                   "MONTO REAL NOT NULL, " 
                   "PAGO_CAPITAL REAL NOT NULL, "
                   "PAGO_INTERES REAL NOT NULL, " 
                   "FECHA_PAGO TEXT NOT NULL, " 
                   "FOREIGN KEY(ID_PRESTAMO) REFERENCES INFO_PRESTAMOS(ID_PRESTAMO));";

    
    rc = sqlite3_exec(db, sql5, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Tabla creada con éxito" << endl;
    }




    // Inserta datos en la tabla
    sql5 = "INSERT INTO PAGO_PRESTAMOS (ID_PAGO, ID_PRESTAMO, MONTO, PAGO_CAPITAL, PAGO_INTERES, FECHA_PAGO) "
        "VALUES (34, 22, 4000.33, 2000.33, 2000.0, '2/6/2023'); ";

    rc = sqlite3_exec(db, sql5, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Records created successfully" << endl;
    }

    // Selecciona datos de la tabla
    sql5= "SELECT * from PAGO_PRESTAMOS";
    rc = sqlite3_exec(db, sql5, callback, (void*)data, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Operation done successfully" << endl;
    }
    
    
 


/*Séptima tabla (Registro de transacciones)*/

    
    const char *sql6 = "CREATE TABLE IF NOT EXISTS TRANSACCIONES("
                   "ID_TRANSACCION INT PRIMARY KEY NOT NULL, "
                   "ID_CUENTA INT NOT NULL, "
                   "ID_CUENTA_DESTINO INT, " 
                   "TIPO_TRANSACCION TEXT NOT NULL, "
                   "MONTO REAL NOT NULL, " 
                   "FECHA_TRANSACCION TEXT NOT NULL, " 
                   "FOREIGN KEY(ID_CUENTA) REFERENCES CUENTAS(ID_CUENTA));";

    
    rc = sqlite3_exec(db, sql6, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Tabla creada con éxito" << endl;
    }




    // Inserta datos en la tabla
    sql6 = "INSERT INTO TRANSACCIONES (ID_TRANSACCION, ID_CUENTA, ID_CUENTA_DESTINO, TIPO_TRANSACCION, MONTO, FECHA_TRANSACCION) "
        "VALUES (345, 3455 , 3456,'Depósito' , 2000.0, '2/6/2023'); ";

    rc = sqlite3_exec(db, sql6, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Records created successfully" << endl;
    }

    // Selecciona datos de la tabla
    sql6= "SELECT * from TRANSACCIONES ";
    rc = sqlite3_exec(db, sql6, callback, (void*)data, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Operation done successfully" << endl;
    }
    
    }


    
/*
    do {

        cout << "\nBienvenido al Banco Central\n";
        cout<< "1. Atención al cliente\n";
        cout<< "2. Información sobre préstamos\n";
        cout<< "3. Salir\n";
        cout<< "Ingrese su opcion: ";
        cin >> opcion;
        //Se utliza un cliclo while en el que se introduce directamente la 
        //entrada en la condición, si se introduce algo no válido como una letra
        //se genera un false, pero el ! lo convierte en true, haciendo que el 
        //ciclo se ejecute, con cin.clear se reestablece cin a su estado vacío
        //y la línea de cin.ignore se encarga de ignorar todos los caracteres en 
        //el buffer de entrada hasta que se llega a una nueva línea. 

        // Se utiliza un switch para ejecutar las diferentes funciones que se tienen en el menú según en número que el usuario introduzca en el menú. 

        switch (opcion)
        {
        case ATENCION:
            
            cout<< "Ingrese su número de cédula: ";
            cin >> cedula;

            cout << "\nSeleccione la opción correspondiente\n";
            cout<< "1. Agregar Cuenta o CDP.\n";
            cout<< "2. Ver información de cuenta o CDP.\n";
            cout<< "3. Realizar transacciones.\n";
            cout<< "4. Opciones de préstamos.\n";
            cout<< "Ingrese su opcion: ";
            cin >> opcionAtencion;

            if (opcionAtencion == 1){

               




            }
            break;
        case INFORMACION:
            
            
           
            break;

        case SALIR:
            
            cout << " \nSaliendo del programa\n";
            break;

        default:


            break;
        }

        // Si la opción no es SALIR, entonces se vuelve a ejecutar el ciclo.
        } while (opcion != SALIR);

        
    

    return 0;

}

*/








