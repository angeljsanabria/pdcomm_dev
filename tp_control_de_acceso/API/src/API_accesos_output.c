/*
 * API_gpio_leds.c
 *
 *  Created on: 18 abr 2026
 *      Author: angel-dev
 */
#include <API_accesos_output.h>
#include "bsp_leds.h"

/*
 * 	Funciones privadas
 */
// Funcionamiento tipo CallBack para manejo de LED
//static void ledOn(void)
//{
//	BSP_LED_On(BSP_LED3);
//	return;
//}
//
//static void ledOff(void)
//{
//	BSP_LED_Off(BSP_LED3);
//	return;
//}
//
//static void ledToggle(void)
//{
//	BSP_LED_Toggle(BSP_LED3);
//	return;
//}

//uint8_t ledIsOn(void)		// Deprecado
//{
//	return BSP_LED_IsOn(BSP_LED3);
//}

static void ledRojoOn(void)
{
	BSP_LED_On(BSP_LED_ROJO);
	return;
}


static void ledAmarilloOn(void)
{
	BSP_LED_On(BSP_LED_AMARILLO);
	return;
}


static void ledVerdeOn(void)
{
	BSP_LED_On(BSP_LED_VERDE);
	return;
}


static void ledRojoOff(void)
{
	BSP_LED_Off(BSP_LED_ROJO);
	return;
}


static void ledAmarilloOff(void)
{
	BSP_LED_Off(BSP_LED_AMARILLO);
	return;
}


static void ledVerdeOff(void)
{
	BSP_LED_Off(BSP_LED_VERDE);
	return;
}


static void ledRojoToggle(void)
{
	BSP_LED_Toggle(BSP_LED_ROJO);
	return;
}
//
//static void ledAmarilloToggle(void)
//{
//	BSP_LED_Toggle(BSP_LED_AMARILLO);
//	return;
//}
//
//static void ledVerdeToggle(void)
//{
//	BSP_LED_Toggle(BSP_LED_VERDE);
//	return;
//}

/*
 * 	Funciones publicas
 */
void acceso_A_abrir(void){
	ledVerdeOn();
}

void acceso_B_abrir(void){
	ledAmarilloOn();
}

void acceso_denegado_indicador(void){
	ledRojoOn();
}

void acceso_error_indicador(void){
	ledRojoToggle();
}

void acceso_cerrar_todos(void){
	ledVerdeOff();
	ledAmarilloOff();
	ledRojoOff();
}
