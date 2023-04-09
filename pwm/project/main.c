// PRIMEIRA ABORDAGEM: PWM COM SW1 E SW2

/*

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"


#define COLOR_PIN GPIO_PIN_1    //VALORES: GPIO_PIN_1 (RED), GPIO_PIN_2 (BLUE), GPIO_PIN_3 (GREEN)
#define PWM_FREQUENCY 55


#ifdef DEBUG
void__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//AUXILIARY FUNCTIONS///////////////////////////////////////////////////////
uint32_t selecionarConfiguracao() {
    switch(COLOR_PIN) {
    case GPIO_PIN_1:
        return GPIO_PF1_M1PWM5;
    case GPIO_PIN_2:
        return GPIO_PF2_M1PWM6;
    case GPIO_PIN_3:
        return GPIO_PF3_M1PWM7;
    default:
        return 0;
    }
}

uint32_t selecionarGeneratorBlock(uint32_t GPIOPinConfigure) {
    switch(GPIOPinConfigure) {
    case GPIO_PF1_M1PWM5:
        return PWM_GEN_2;
    case GPIO_PF2_M1PWM6:
    case GPIO_PF3_M1PWM7:
        return PWM_GEN_3;
    default:
        return 0;
    }
}

uint32_t selecionarPWMOutput(uint32_t GPIOPinConfigure) {
    switch(GPIOPinConfigure) {
    case GPIO_PF1_M1PWM5:
        return PWM_OUT_5;
    case GPIO_PF2_M1PWM6:
        return PWM_OUT_6;
    case GPIO_PF3_M1PWM7:
        return PWM_OUT_7;
    default:
        return 0;
    }
}

uint32_t selecionarPWMOutputBit(uint32_t GPIOPinConfigure) {
    switch(GPIOPinConfigure) {
    case GPIO_PF1_M1PWM5:
        return PWM_OUT_5_BIT;
    case GPIO_PF2_M1PWM6:
        return PWM_OUT_6_BIT;
    case GPIO_PF3_M1PWM7:
        return PWM_OUT_7_BIT;
    default:
        return 0;
    }

}
////////////////////////////////////////////////////////////////////////

int main(void)
{

    volatile uint32_t ui32Load;
    volatile uint32_t ui32PWMClock;
    volatile uint8_t ui8Adjust;
    ui8Adjust = 83;

    //VARIAVEIS DE SUPORTE//////////////////////////////////////////////////
    uint32_t GPIOPinConfigure = selecionarConfiguracao();
    uint32_t generatorBlock = selecionarGeneratorBlock(GPIOPinConfigure);
    uint32_t PWMOutput = selecionarPWMOutput(GPIOPinConfigure);
    uint32_t PWMOutputBit = selecionarPWMOutputBit(GPIOPinConfigure);
    ////////////////////////////////////////////////////////////////////////

    ROM_SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    ROM_SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    ROM_GPIOPinTypePWM(GPIO_PORTF_BASE, COLOR_PIN);
    ROM_GPIOPinConfigure(GPIOPinConfigure);

    //HABILITAR SW1 E SW2 BUTTONS
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
    ROM_GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
    ROM_GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    ui32PWMClock = SysCtlClockGet() / 64;
    ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;
    PWMGenConfigure(PWM1_BASE, generatorBlock, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM1_BASE, generatorBlock, ui32Load);

    ROM_PWMPulseWidthSet(PWM1_BASE, PWMOutput, ui8Adjust * ui32Load / 1000);
    ROM_PWMOutputState(PWM1_BASE, PWMOutputBit, true);
    ROM_PWMGenEnable(PWM1_BASE, generatorBlock);

    while(1)
    {

        if(ROM_GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)
        {
            ui8Adjust--;
            if (ui8Adjust < 56)
            {
                ui8Adjust = 56;
            }
            ROM_PWMPulseWidthSet(PWM1_BASE, PWMOutput, ui8Adjust * ui32Load / 1000);
        }

        if(ROM_GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00)
        {
            ui8Adjust++;
            if (ui8Adjust > 111)
            {
                ui8Adjust = 111;
            }
            ROM_PWMPulseWidthSet(PWM1_BASE, PWMOutput, ui8Adjust * ui32Load / 1000);
        }

        ROM_SysCtlDelay(100000);
    }

}

*/

// SEGUNDA ABORDAGEM: PWM PULSE

/*################################################
# Hardware PWM proof of concept using
# the Tiva C Launchpad
#
# Started with example code by
# lawrence_jeff found here:
# http://forum.stellarisiti.com/topic/707-using-hardware-pwm-on-tiva-launchpad/
#
# Altered to use code found on section
# 22.3 of the TivaWare Peripheral Driver
# Library User's Guide found here:
# http://www.ti.com/lit/ug/spmu298a/spmu298a.pdf
#
#
# This example pulses three on-board LEDs
#
#################################################*/

///*

#include "driverlib/pin_map.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"

void delayMS(int ms) {
    SysCtlDelay( (SysCtlClockGet()/(3*1000))*ms ) ;
}

int
main(void)
{
   //Set the clock
   SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC |   SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

   //Configure PWM Clock to match system
   SysCtlPWMClockSet(SYSCTL_PWMDIV_1);

   // Enable the peripherals used by this program.
   SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
   SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);  //The Tiva Launchpad has two modules (0 and 1). Module 1 covers the LED pins

    //Configure PF1,PF2,PF3 Pins as PWM
    GPIOPinConfigure(GPIO_PF1_M1PWM5);
    GPIOPinConfigure(GPIO_PF2_M1PWM6);
    GPIOPinConfigure(GPIO_PF3_M1PWM7);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);

    //Configure PWM Options
    //PWM_GEN_2 Covers M1PWM4 and M1PWM5
    //PWM_GEN_3 Covers M1PWM6 and M1PWM7 See page 207 4/11/13 DriverLib doc
    PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

    //Set the Period (expressed in clock ticks)
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, 320);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, 320);

    //Set PWM duty-50% (Period /2)
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,100);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,100);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,100);

    // Enable the PWM generator
    PWMGenEnable(PWM1_BASE, PWM_GEN_2);
    PWMGenEnable(PWM1_BASE, PWM_GEN_3);

    // Turn on the Output pins
    PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT | PWM_OUT_6_BIT | PWM_OUT_7_BIT, true);
    PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);

    //Fade
    bool fadeUp = true;
    unsigned long increment = 10;
    unsigned long pwmNow = 100;
    while(1)
    {
        delayMS(20);
        if (fadeUp) {
            pwmNow += increment;
            if (pwmNow >= 320) { fadeUp = false; }
        }
        else {
            pwmNow -= increment;
            if (pwmNow <= 10) { fadeUp = true; }
        }
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,pwmNow);
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,pwmNow);
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,pwmNow);
    }

}


//*/
