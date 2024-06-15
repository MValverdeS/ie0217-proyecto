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







    // Cierra la base de datos

    sqlite3_close(db);
    return 0;
}




/*
enum opciones { 
    ATENCION= 1,
    INFORMACION,
    SALIR

    };





int main() {
   
    // Se declara la variable que contendrá la opción introducida por el usuario. 
    
    int opcion;
    int cedula;
    int opcionAtencion;


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

            cout << "\nOpción no valida, intente de nuevo \n";

            break;
        }

        // Si la opción no es SALIR, entonces se vuelve a ejecutar el ciclo.
        } while (opcion != SALIR);

        
    

    return 0;

}

*/








