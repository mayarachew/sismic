;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file
            
;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory.
            .retain                         ; Override ELF conditional linking
                                            ; and retain current section.
            .retainrefs                     ; And retain any sections that have
                                            ; references to current section.

;-------------------------------------------------------------------------------
RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer

;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------

main:
			; Deixa como 0 todos os BIT's
			bic.b 	#(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7),P1DIR
			bic.b 	#(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7),P1OUT

			; P1.0 - LED vermelho
			; P1.0 = Porta 1 Pino 0

			; DIR = 1 (saída)
			; DIR = 2 (entrada)

			; é .b pois cada porta agrupa apenas 8 pinos

			; Configura P1.0 como saída
			bis.b 	#BIT0,P1DIR				; P1.0 é uma saída

			bic.b	#BIT0,P1OUT				; Zera o bit de saída da porta 1
											; LED apaga
			bis.b	#BIT0,P1OUT				; LED acende
			bic.b	#BIT0,P1OUT				; LED apaga
			bis.b	#BIT0,P1OUT				; LED acende

			; xor.b atua como uma inversora
			xor.b	#BIT0,P1OUT				; LED apaga
			xor.b	#BIT0,P1OUT				; LED acende
			xor.b	#BIT0,P1OUT				; LED apaga
			xor.b	#BIT0,P1OUT				; LED acende

			; Acessar P1.0 até P1.7 e P2.0 a P2.7
			mov.w 	#0,PADIR				; PA = P1 | P2

			bis.b 	#(BIT2|BIT3),P1DIR
			mov.b 	#BIT2,P1OUT				; Seta o BIT2 como 1 e os outros como 0

			bic.b 	#(BIT2|BIT3|BIT4|BIT5),P1OUT
			bis.b 	#(BIT2|BIT3|BIT4|BIT5),P1OUT

			bic.b 	#(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7),P1DIR
			bic.b 	#(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7),P1OUT

			bis.b 	#BIT0,P1DIR

; Leitura dos pinos
; SW1 - P2.1
; SW2 - P1.1

			; Configurando P2.1 como entrada
			; P2.1 = Porta 2 Pino 1
			bic.b	#BIT1,P2DIR				; P2.1 é entrada (DIR = 0)
			bis.b	#BIT1,P2REN				; P2.1 tem resistor (REN = 1)
			bis.b	#BIT1,P2OUT				; Resistor de pull-up

			;bic.b	#BIT1,P2OUT				; Resistor de pull-down

loop:
			bit.b	#BIT1,P2IN				; (P2IN & BIT1)
											; & entre a máscara e o registro

			; Botão solto => carry = 1
			; Botão pressionado => carry = 0

			jc		botaoSolto

botaoPressionado:
			bis.b	#BIT0,P1OUT				; LED aceso
			jmp 	loop

botaoSolto:
			bic.b	#BIT0,P1OUT				; LED apagado
			jmp		loop

			;jmp $
			nop

;-------------------------------------------------------------------------------
;	Memória RAM
;-------------------------------------------------------------------------------

;-------------------------------------------------------------------------------
; Stack Pointer definition
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack
            
;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET
            

