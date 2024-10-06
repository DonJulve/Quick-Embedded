/******************************************************************
* Area Ingenieria de Sistemas y 
* Automatica y Arquitectura y tecnologia de Computadores
* Sistemas Empotrados 1
*******************************************************************
* nombre fichero : main.c
*******************************************************************
* descripcion    : Practica PR1 comprobación placa
* programador    : JL Villarroel (JLV) & Enrique Torres
* lenguaje       : ANSI C
* fecha          : 8/03/2022
********************************************************************/

/******************************************************************
*                   Modulos usados
*******************************************************************/
#include <stdlib.h>
#include <msp430.h>
#include "InitSystem.h"
#include "display.h"

/******************************************************************
*                   Variables compartidas
*******************************************************************/

static volatile unsigned char pulso_flag = 0;
static volatile unsigned char j = 0;

/******************************************************************
*                   Prototipos funciones
*******************************************************************/
void Init_GPIO (void) ;

/******************************************************************
*                   Main y funciones
*******************************************************************/
int main(void)
{
    static unsigned char i = 0, k = 0, antes_pulsado = 0, antes_pulsado_2 = 0, antes_pulsado_3 = 0, antes_pulsado_4 = 0, antes_pulsado_5 = 0, antes_pulsado_6 = 0 ;

    Stop_Watchdog () ;                  // Stop watchdog timer

    Init_CS () ;                        // MCLK = 8 MHz, SMCLK = 4 MHz
    Init_GPIO () ;
    Init_Display () ;

    P1IE |= BIT1 | BIT2;                    // P1.1 Int enabled
    P1IES &= ~(BIT1 | BIT2) ;                  // P1.1 Int raising edge

    Init_Clock();
    unsigned int tiempo = 1;
    while (tiempo < 8){
        switch(tiempo){
                        case 1:
                            P2OUT |= BIT0;
                            break;
                        case 2:
                            P2OUT |= BIT1;
                            break;
                        case 3:
                            P2OUT |= BIT2;
                            break;
                        case 4:
                            P4OUT |= BIT4;
                            break;
                        case 5:
                            P4OUT |= BIT5;
                            break;
                        case 6:
                            P4OUT |= BIT6;
                            break;
                        case 7:
                            P4OUT |= BIT7;
                            break;
                        default:
                            break;
        }
       delay_until(500 + Get_Time());
       tiempo++;
    }

    while(1)
    {
        if (P1IN & BIT1)
            P2OUT &= ~BIT0 ;
        else P2OUT |= BIT0 ;

        if (P1IN & BIT2)
            P2OUT &= ~BIT1;
        else P2OUT |= BIT1;

        if (P1IN & BIT3)
            P2OUT &= ~BIT2;
        else P2OUT |= BIT2;

        if (P1IN & BIT4)
            P4OUT &= ~BIT4;
        else P4OUT |= BIT4;

        if (P1IN & BIT5)
            P4OUT &= ~BIT5;
        else P4OUT |= BIT5;

        if (P1IN & BIT6)
            P4OUT &= ~BIT6;
        else P4OUT |= BIT6;

/*
        if ((P1IN & BIT1) && !antes_pulsado) {//detector de flancos
            antes_pulsado = 1;
            j = (j + 1)%10 ;
        }
        else if (!(P1IN & BIT1)) antes_pulsado = 0 ;

        if ((P1IN & BIT2) && !antes_pulsado_2) {//detector de flancos
            antes_pulsado_2 = 1;
            if (j > 0) j--;
        }
        else if (!(P1IN & BIT2)) antes_pulsado_2 = 0 ;

*/
        if ((P1IN & BIT3) && !antes_pulsado_3) {//detector de flancos
            antes_pulsado_3 = 1;
            i = (i + 1)%10 ;
        }
        else if (!(P1IN & BIT3)) antes_pulsado_3 = 0 ;

        if ((P1IN & BIT4) && !antes_pulsado_4) {//detector de flancos
            antes_pulsado_4 = 1;
            if (i > 0) i--;
        }
        else if (!(P1IN & BIT4)) antes_pulsado_4 = 0 ;

        if ((P1IN & BIT5) && !antes_pulsado_5) {//detector de flancos
            antes_pulsado_5 = 1;
            k = (k + 1)%10 ;
        }
        else if (!(P1IN & BIT5)) antes_pulsado_5 = 0 ;

        if ((P1IN & BIT6) && !antes_pulsado_6) {//detector de flancos
            antes_pulsado_6 = 1;
            if (k > 0) k--;
        }
        else if (!(P1IN & BIT6)) antes_pulsado_6 = 0 ;

        display (2, j) ;
        display (1, i);
        display (0, k);


        if (pulso_flag > 0){
            pulso_flag = 0;     // OjO, secion critica? 

        } 
    }
}


void Init_GPIO (void) {
    // P2.6,P2.7: XIN, XOUT, reloj externo
    // P1.0 salida, led de la tarjeta
    // P6.6 salida, led de la tarjeta
    // P1.1 .. P1.6 entradas, pulsadores del ascensor
    // P3 control display 7 segmentos
    // P6.0 y P6.1 --> entradas encoder
    // P6.2 y P6.3 --> salidas PWM
    // resto: sin uso

    P1DIR |= BIT0 ;                                          // led de la tarjeta
    // P1.1 - P1.6 entradas para pulsadores
    P1REN |= BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 ;       // pull-down/pull-up
    P1OUT |= BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 ;       // pull-up
    P1DIR |= BIT7 ;                                          // sin uso
    // P2.0 - P2.2 salidas para led
    P2DIR |= BIT0 | BIT1 | BIT2  ;                           // salida led
    P2DIR |= BIT3 | BIT4 | BIT5 ;                            // sin uso
    P2OUT = 0x00 ;                                           // sin uso
    // P3 control display 7 segmentos
    P3DIR |= BIT0 | BIT1 | BIT2 ;                            // D0, D1, D2
    P3DIR |= BIT3 | BIT4 | BIT5 | BIT6 ;                     // A, B, C, D
    P3DIR |= BIT7 ;                                          // sin uso
    P3OUT = 0x00 ;
    // P4.5 - P4.7 salidas para led
    P4DIR |= BIT0 | BIT1 | BIT2 | BIT3 | BIT4 ;              // sin uso
    P4DIR |= BIT5 | BIT6 | BIT7 ;                            // salida led
    P4OUT = 0x00 ;
    // P5.2 - P5.3 ADC
    P5DIR |= BIT0 | BIT1 | BIT4 ;                            // sin uso
    P5OUT = 0x00 ;
    // P6.0 - P6.1 entradas encoder
    // P6.2 - P6.3 salidas PWM
    P6DIR |= BIT2 | BIT3 ;                                   // PWM
    P6DIR |= BIT4 | BIT5 ;                                   // sin uso
    P6DIR |= BIT6 ;                                          // led de la tarjeta
    P6OUT = 0x00 ;


    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings
}

#pragma vector = PORT1_VECTOR
__interrupt void RSI_Pulso (void) {
    unsigned int Interrupt;
    pulso_flag++;
    Interrupt = P1IV;
    if (Interrupt) {
        if (Interrupt == 0x4)
            j = (j + 1)%10;

        else if ((Interrupt == 0x6) && j > 0) j--;
    }
    return ;
}

