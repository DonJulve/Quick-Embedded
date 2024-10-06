/******************************************************************************/
/*                                                                            */
/*                 Area Ingenieria de Sistemas y Automatica                   */
/*           Departamento de Informatica e Ingenieria de Sistemas             */
/*                         Universidad de Zaragoza                            */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/* proyecto  : PRACTICAS SEP - SEI UNIZAR                                     */
/* fichero   : main.c                                                          */
/* version   : 1                                                              */
/* fecha     : 22/02/2022                                                     */
/* autor     : Jose Luis Villarroel                                           */
/* descripcion : Practica PR5 (lavadora) (MSP430)                             */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                        Modulos usados                                      */
/******************************************************************************/

#include <msp430.h>
#include "InitSystem.h"
#include "clock.h"
#include "pwm.h"
#include "encoder.h"
#include "servos.h"
#include "ad.h"
#include "display.h"


/******************************************************************************/
/*                        Variables globales                                  */
/******************************************************************************/

unsigned int siguiente, periodo, i = 0, j = 0, en_marcha = 0, Display = 0, paso = 0, programa = 0, comenzar = 0, parar = 0, empezo = 0;
float W, Wref;
float velocidad = 0, centrifugado = 0;

Timer_id Ti ;

// Variables globales de la practica

/******************************************************************************/
/*                        Prototipos funciones                                */
/******************************************************************************/

void Init_GPIO (void) ;

/******************************************************************************/
/*                        Main y funciones                                    */
/******************************************************************************/

void Programa_1(void){
    Remove_Timer(Ti);
    switch(paso){
                    case 0:
                        en_marcha = 0;
                        velocidad = 0;
                        P2OUT &= ~BIT0;
                        empezo = 0;
                        comenzar = 0;
                        break;
                    case 1:
                        velocidad = centrifugado;
                        paso--;
                        Ti = Set_Timer(4000, 0, Programa_1);
                        break;
                    case 2:
                        velocidad = 0;
                        paso--;
                        Ti = Set_Timer(4000, 0, Programa_1);
                        break;
                    case 3:
                        velocidad = 1;
                        paso--;
                        Ti = Set_Timer(4000, 0, Programa_1);
                        break;
                    case 4:
                        velocidad = -1;
                        paso--;
                        Ti = Set_Timer(4000, 0, Programa_1);
                        break;
                    case 5:
                        velocidad = 1;
                        paso--;
                        Ti = Set_Timer(10000, 0, Programa_1);
                        break;
    }
}

void Programa_2(void){
    Remove_Timer(Ti);
    switch(paso){
                    case 0:
                        en_marcha = 0;
                        velocidad = 0;
                        P2OUT &= ~BIT0;
                        empezo = 0;
                        comenzar = 0;
                        break;
                    case 1:
                        velocidad = centrifugado;
                        paso--;
                        Ti = Set_Timer(6000, 0, Programa_2);
                        break;
                    case 2:
                        velocidad = 0;
                        paso--;
                        Ti = Set_Timer(2000, 0, Programa_2);
                        break;
                    case 3:
                        velocidad = 1;
                        paso--;
                        Ti = Set_Timer(4000, 0, Programa_2);
                        break;
                    case 4:
                        velocidad = -0.5;
                        paso--;
                        Ti = Set_Timer(2000, 0, Programa_2);
                        break;
    }
}

void Programa_3(void){
    Remove_Timer(Ti);
    switch(paso){
                    case 0:
                        en_marcha = 0;
                        velocidad = 0;
                        P2OUT &= ~BIT0;
                        empezo = 0;
                        comenzar = 0;
                        break;
                    case 1:
                        velocidad = centrifugado;
                        paso--;
                        Ti = Set_Timer(3000, 0, Programa_3);
                        break;
                    case 2:
                        velocidad = -centrifugado;
                        paso--;
                        Ti = Set_Timer(3000, 0, Programa_3);
                        break;
    }
}

void Control_discreto(void){
    if (en_marcha){

        if(empezo == 0){
        switch(programa){
            case 1:
                   Programa_1 ();
                   break;
            case 2:
                    Programa_2 ();
                    break;
            case 3:
                    Programa_3 ();
                    break;

        }
        empezo = 1;
        }
        if (parar){
            P2OUT &= ~BIT0;
            en_marcha = 0;
            velocidad = 0;
            empezo = 0;
            comenzar = 0;
            Remove_Timer(Ti);
        }
    }
    else{
        if (comenzar){
            P2OUT |= BIT0;
            en_marcha = 1;
            parar = 0;
        }
        if (P1IN & BIT2){
            P2OUT |= BIT1;
            P2OUT &= ~BIT2;
            P4OUT &= ~BIT4;
            programa = 1;
            paso = 5;
        }
        if (P1IN & BIT3){
            P2OUT |= BIT2;
            P2OUT &= ~BIT1;
            P4OUT &= ~BIT4;
            programa = 2;
            paso = 4;
        }
        if (P1IN & BIT4){
            P4OUT |= BIT4;
            P2OUT &= ~BIT1;
            P2OUT &= ~BIT2;
            programa = 3;
            paso = 2;
        }
        if (P1IN & BIT5){
            P4OUT |= BIT5;
            P4OUT &= ~BIT6;
            centrifugado = 2.5;
        }
        if (P1IN & BIT6){
            P4OUT |= BIT6;
            P4OUT &= ~BIT5;
            centrifugado = 4;
        }
    }
}

void Mostrar_display(void){
    switch(Display){
        case 0:
            display (Display, paso);
            break;
        case 1:
            display (Display, programa);
            break;
    }
    Display = (Display + 1) % 2;
}
int main(void)
{
    Stop_Watchdog () ;                  // Stop watchdog timer

    Init_CS () ;                        // MCLK = 8 MHz, SMCLK = 4 MHz
    Init_Clock () ;
    Init_GPIO () ;
    Init_Display ();

    Init_PWM () ;
    Init_Encoder () ;
    Init_AD () ;

    P1IE |= BIT1 ;
    P1IES &= ~BIT1 ;

    periodo = 100;


    Init_Servos (periodo) ;

    unsigned int aux = 0;
    periodo = 10;

    siguiente = Get_Time () ;


    while(1)
    {


        Mostrar_display();
        if (aux == 5){
            Control_discreto();
        }
        else if (aux == 10){
            Control_discreto();
            Wref = velocidad;
            //Wref = 0.00195*(float)Read_Value_Int_1()-4.0;
            W = velocity();
            action(R(Wref, W));
            aux = 0;
        }
        aux++;
        siguiente += periodo ;
        delay_until (siguiente) ;
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
    Interrupt = P1IV;
    if (Interrupt) {
        if (Interrupt == 0x4){
            if (en_marcha == 0)
                      comenzar = 1;
                  else
                      parar = 1;
        }
    }
    return ;
}
