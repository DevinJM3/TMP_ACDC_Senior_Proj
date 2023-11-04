#include "ACDC_INTERRUPT.h"
#include "ACDC_GPIO.h"

void INTERRUPT_Enable(IRQn_Type IRQn){
    //Enables Specific Interrupt Vectors {See PM-120}
    if(IRQn >= 0)
        NVIC->ISER[IRQn >> 5] |= 0b1 << (IRQn & 0x1F);
}

void INTERRUPT_Disable(IRQn_Type IRQn){
    //Disables Specific Interrupt Vectors {See PM-121}
    if(IRQn >= 0)
        NVIC->ICER[IRQn >> 5] |= 0b1 << (IRQn & 0x1F);
}

void INTERRUPT_SetPriority(IRQn_Type IRQn, uint8_t PreemptPriority, uint8_t SubPriority){
    //https://electronics.stackexchange.com/q/384312
    //Sets the Priority of an interrupt {See PM-125}
}

void GPIO_INT_SetToInterrupt(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN, GPIO_TriggerType triggerType){
    //External Interrupts are Muxed so you can only use 16 concurrently {See Page 210 RM}
    //https://community.st.com/t5/stm32-mcus-products/nvic-interrupts-on-two-ports-with-same-pin-number/td-p/550469

    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; //Enable Alternate Functions (Needed for Interrupts)

    uint16_t pin = GPIO_GetPinNumber(GPIO_PIN);
    EXTI->IMR |= 1 << pin;        //Enable Interrupts on pin
    if(triggerType & RISING_EDGE)
        EXTI->RTSR |= 1 << pin;   //Enable RisingEdge Interrupts on pin
    if(triggerType & FALLING_EDGE)
        EXTI->FTSR |= 1 << pin;   //Enable FallingEdge Interrupts on pin

    uint16_t CRNumber = pin >> 2; //Gets the EXTICRx number
    uint16_t GpioMask;            //Gets the GPIO mask {See RM-191}
    if(GPIOx == GPIOA)
        GpioMask = 0b0000;
    else if(GPIOx == GPIOB)
        GpioMask = 0b0001;
    else if(GPIOx == GPIOC)
        GpioMask = 0b0010;
    else if(GPIOx == GPIOD)
        GpioMask = 0b0011;
    else if(GPIOx == GPIOE)
        GpioMask = 0b0100;

    AFIO->EXTICR[CRNumber] &= ~(0b1111 << (pin % 4)*4); //Clear Register First 
    AFIO->EXTICR[CRNumber] |= GpioMask << (pin % 4)*4;  //Set Register to Value  
}