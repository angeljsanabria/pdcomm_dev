/*
 * API_accesos_output.h
 *
 *  Created on: 18 abr 2026
 *      Author: angel-dev
 */

#ifndef API_INC_API_ACCESOS_OUTPUT_H_
#define API_INC_API_ACCESOS_OUTPUT_H_

/**
* @brief 	output de acceso A abierto
*
* @param 	None
* @retval 	None
*/
void acceso_A_abrir(void);


/**
* @brief 	output de acceso B abierto
*
* @param 	None
* @retval 	None
*/
void acceso_B_abrir(void);


/**
* @brief 	Indicado de NO ACCESO ON
*
* @param 	None
* @retval 	None
*/
void acceso_denegado_indicador(void);


/**
* @brief 	Indicado de ERROR
*
* @param 	None
* @retval 	None
*/
void acceso_error_indicador(void);

/**
* @brief 	Cierro todos los accesos
*
* @param 	None
* @retval 	None
*/
void acceso_cerrar_todos(void);


#endif /* API_INC_API_ACCESOS_OUTPUT_H_ */
