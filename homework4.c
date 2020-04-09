#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "homework4.h"

int main(void)
{
    char rChar;
    char *response = "\n\n\r2534 is the best course in the curriculum!\r\n\n";

    // TODO: Declare the variables that main uses to interact with your state machine.
    bool success = false;
    int count;

    // Stops the Watchdog timer.
    initBoard();
    // TODO: Declare a UART config struct as defined in uart.h.
    //       To begin, configure the UART for 9600 baud, 8-bit payload (LSB first), no parity, 1 stop bit.
    eUSCI_UART_ConfigV1 myUARTConfig = {
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,
        19,
        8,
        0xAA,
        EUSCI_A_UART_NO_PARITY,
        EUSCI_A_UART_LSB_FIRST,
        EUSCI_A_UART_ONE_STOP_BIT,
        EUSCI_A_UART_MODE,
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,
    };

    // TODO: Make sure Tx AND Rx pins of EUSCI_A0 work for UART and not as regular GPIO pins.
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    // TODO: Initialize EUSCI_A0
    UART_initModule(EUSCI_A0_BASE, &myUARTConfig);

    // TODO: Enable EUSCI_A0
    UART_enableModule(EUSCI_A0_BASE);

    while(1)
    {
        // TODO: Check the receive interrupt flag to see if a received character is available.
        //       Return 0xFF if no character is available.
        if (!UART_getInterruptStatus(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG))
            rChar = 0xFF;

        // TODO: If an actual character was received, echo the character to the terminal AND use it to update the FSM.
        //       Check the transmit interrupt flag prior to transmitting the character.
        else {
            if (UART_getInterruptStatus(EUSCI_A0_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG)) {
                rChar = UART_receiveData(EUSCI_A0_BASE);
                UART_transmitData(EUSCI_A0_BASE, rChar);
            }

            success = charFSM(rChar);
        }


        // TODO: If the FSM indicates a successful string entry, transmit the response string.
        //       Check the transmit interrupt flag prior to transmitting each character and moving on to the next one.
        //       Make sure to reset the success variable after transmission.
        if (success) {
                for (count = 0; count < 49; count++) {
                    while (!UART_getInterruptStatus(EUSCI_A0_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG)) {
                    }

                    UART_transmitData(EUSCI_A0_BASE, *response);
                    *response++;
                }

            success = false;
            response = "\n\n\r2534 is the best course in the curriculum!\r\n\n";
        }
    }
}

void initBoard()
{
    WDT_A_hold(WDT_A_BASE);
}

// TODO: FSM for detecting character sequence.
bool charFSM(char rChar)
{
    bool finished = false;
    typedef enum {FIRST, SECOND, THIRD, FOURTH} charState;
    static charState cState = FIRST;

    switch (cState) {
        case FIRST:
            if (rChar == '2') {
                cState = SECOND;
                finished = false;
            }
            else {
                cState = FIRST;
                finished = false;
            }
            break;
        case SECOND:
            if (rChar == '5') {
                cState = THIRD;
                finished = false;
            }
            else if (rChar == '2') {
                cState = SECOND;
                finished = false;
            }
            else if (rChar == 0xFF) {
                cState = SECOND;
                finished = false;
            }
            else {
                cState = FIRST;
                finished = false;
            }
            break;
        case THIRD:
            if (rChar == '3') {
                cState = FOURTH;
                finished = false;
            }
            else if (rChar == '2') {
                cState = SECOND;
                finished = false;
            }
            else if (rChar == 0xFF) {
                cState = THIRD;
                finished = false;
            }
            else {
                cState = FIRST;
                finished = false;
            }
            break;
        case FOURTH:
            if (rChar == '4') {
                cState = FIRST;
                finished = true;
            }
            else if (rChar == '2') {
                cState = SECOND;
                finished = false;
            }
            else if (rChar == 0xFF) {
                cState = FOURTH;
                finished = false;
            }
            else {
                cState = FIRST;
                finished = false;
            }
            break;
    }

    return finished;
}
