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
config:
			; Configura os pinos
			bic.b	#BIT0,P1OUT		; Configura P1.0 (LED vermelho)
			bis.b	#BIT0,P1DIR		; Como saída

			bic.b	#BIT7,P4OUT		; Configura P4.7 (LED verde)
			bis.b	#BIT7,P4DIR		; Como saída

			; Como são pinos de saída, é melhor definir
			; a saída (OUT) antes de definir a direção (DIR)

            bic.b	#BIT1,P1DIR		; Configura P1.1
            bis.b	#BIT1,P1REN		; Como entrada ; Habilita o resistor
            bis.b	#BIT1,P1OUT		; Resistor de pull-up

            bic.b	#BIT1,P2DIR		; Configura P2.1
            bis.b	#BIT1,P2REN		; Como entrada ; Habilita o resistor
            bis.b	#BIT1,P2OUT		; Resistor de pull-up

main:
			; Loop infinito

tarefaImportante:
			xor.b	#BIT0,P1OUT		; Pisca o LED vermelho
			call	#delay

amostraBT:
; R5 = BT(n) -> BT = P1.1
; R6 = BT(n-1)
			mov.b	R5,R6			; BT(n-1) <= BT(n)
			mov.b	P1IN,R5			; Amostra um novo BT(n)
			and.b	#BIT1,R5		; R5 é BT(n), R6 é BT(n-1) ; Filtra apenas o pino 1 da porta 1

			bit.b	#BIT1,R5		; ; Ou bit.b	#BIT1,P1IN
			jc		BTsolto

BTpressionado:
			bit.b	#BIT1,R6
			jnc		tarefaImportante

			xor.b	#BIT7,P4OUT		; Ação ao pressionar ; Alterna LED verde

			call	#delay
			jmp 	tarefaImportante

BTsolto:
			bit.b	#BIT1,R6
			jc		tarefaImportante

			; Ação quando BT for solto
			call	#delay
			jmp 	tarefaImportante

			jmp 	main

;-------------------------------------------------------------------------------
; Debounce com trava de execução

esperaApertar:
			bit.b	#BIT1,P1IN		; Espera o usuário apertar P1.1
			jc		esperaApertar	; Trava de execução

			xor.b	#BIT0,P1OUT		; Alterna o estado do LED vermelho

			call	#delay

esperaSoltar:
			bit.b	#BIT1,P1IN		; Espera o usuário apertar P1.1
			jnc		esperaSoltar	; Trava de execução

			call	#delay

			jmp 	main

;-------------------------------------------------------------------------------

delay:
			push	R4
			mov		#50000,R4

delay_loop:
			dec 	R4
			jnz		delay_loop

delay_ret:
			pop		R4
			ret

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
            

