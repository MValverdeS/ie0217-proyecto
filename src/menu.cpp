/**
 * @file  menu.cpp
 * @brief Se definen los diferentes menús
 *
 * @author Gabriel González Rivera B93432
 * @author Edgar Marcelo Valverde Solís C08089
 * @author Daniel Rodríguez Rivas B96719
 * @date 30/6/2024
 * 
 * Licencia: MIT
 */



#include <iostream>
#include "menu.hpp"

using namespace std;




/**
* @brief función tipo void que imprime el menú inicial
* 
* 
*/

void menuInicial() {
    cout << "Bienvenido al Sistema de Gestion Bancaria" << endl;
    cout << "Seleccione el modo de operacion:" << endl;
    cout << "1. Atencion a Clientes" << endl;
    cout << "2. Informacion General sobre Prestamos" << endl;
    cout << "3. Salir" << endl;
}

/**
* @brief función tipo void que imprime el menú de atención a clientes
* 
* 
*/
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


/**
* @brief función tipo void que imprime el menú de información de préstamos
* 
* 
*/
void menuInformacionPrestamos() {
    cout << "Menu de Informacion General sobre Prestamos" << endl;
    cout << "1. Ver Informacion de Prestamos" << endl;
    cout << "2. Calcular Tabla de Pagos" << endl;
    cout << "3. Agregar Tipo de Prestamo" << endl;
    cout << "4. Salir" << endl;
}


/**
* @brief función tipo void que imprime el menú de acciones de préstamos
* 
* 
*/
void menuPrestamos() {
    cout << "Menu de Prestamos" << endl;
    cout << "1. Solicitar Prestamo" << endl;
    cout << "2. Ver Prestamos" << endl;
    cout << "3. Generar Reporte de Prestamos" << endl;
    cout << "4. Pagar Prestamo" << endl;
    cout << "5. Salir" << endl;
}


/**
* @brief función tipo void que imprime el menú de CDP
* 
* 
*/
void menuCDP() {
    cout << "Menu de CDP" << endl;
    cout << "1. Crear CDP" << endl;
    cout << "2. Ver CDP" << endl;
    cout << "3. Eliminar CDP" << endl;
    cout << "4. Salir" << endl;
}