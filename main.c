/*
 * main.c
 */

#include "DFPlayer/DFPlayer.h"
#include "inc/hw_gpio.h"

int16_t i16_test = 0;

void ButtonISR(void)
{
    uint32_t ui32_IntStatus;
    ui32_IntStatus = GPIOIntStatus(GPIO_PORTF_BASE, true);
    GPIOIntClear(GPIO_PORTF_BASE, ui32_IntStatus);
    if(ui32_IntStatus & GPIO_PIN_4){
        //DFP_prev();
        DFP_get_volume();
        i16_test = mp3_wait_volume();
    }
    if(ui32_IntStatus & GPIO_PIN_0){
        DFP_next();
    }
}

void main(void) {
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //enable GPIO port for LED
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2); //enable pin for LED PF2

    //Config Buttons
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;  //Unlocks the GPIO Commit (GPIOCR) register for write access PF0
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= GPIO_PIN_0;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
    GPIODirModeSet(GPIO_PORTF_BASE,  GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    //Config GPIO Interrupt for SW1
    GPIOIntTypeSet(GPIO_PORTF_BASE,GPIO_PIN_4|GPIO_PIN_0, GPIO_FALLING_EDGE);
    GPIOIntRegister(GPIO_PORTF_BASE,&ButtonISR);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0);

    IntEnable(INT_GPIOF);
    IntMasterEnable();

    DFP_Init();

    DFP_set_volume(15);
    DFP_play();

	while(1){
	    SysCtlDelay(SysCtlClockGet());
	    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
//	    DFP_play();
	    SysCtlDelay(SysCtlClockGet());
	    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
//	    DFP_pause();
	}
}
