#include <stdint.h>
#include <stdbool.h>
#include "utils/ustdlib.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/hibernate.h"
#include "driverlib/gpio.h"

int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ| SYSCTL_OSC_MAIN); // CLOCK DE 40MHz

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x08);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_HIBERNATE); // enable the hibernation module
    HibernateEnableExpClk(SysCtlClockGet()); // defines the clock supplied to the hibernation module
    HibernateGPIORetentionEnable(); // Calling this function enables the GPIO pin state to be maintained during hibernation and remain active even when waking from hibernation.
    SysCtlDelay(64000000); // 4 SEGUNDOS DE DELAY PARA HIBERNAR
    HibernateWakeSet(HIBERNATE_WAKE_PIN | HIBERNATE_WAKE_RTC); // set the wake condition to the wake pin: BOTÃO SW2 OU RTC

    HibernateRTCSet(0);
    HibernateRTCEnable();
    HibernateRTCMatchSet(0, 5); // Acorda a placa depois de 5 segundos

    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00); // turn off the green LED before the device goes to sleep
    HibernateRequest(); // REQUISIÇÃO PARA ATIVAR O MODO HIBERNAÇÃO

    while (1) // ARTIFÍCIO PARA DEIXAR A PLACA LIGADA, MAS HIBERNANDO
    {
    }


}
