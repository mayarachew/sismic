;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file
            .def    RESET                   ; Export program entry-point
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory.

RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
		    mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer

;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
config:
			; configure aqui os pinos dos botoes e LEDs
			bic.b	#BIT0,P1OUT				; P1.0 (LED vermelho)
			bis.b	#BIT0,P1DIR				; Configura como saída

			bic.b	#BIT7,P4OUT				; P4.7 (LED verde)
			bis.b	#BIT7,P4DIR				; Configura como saída

			bic.b	#BIT1,P1DIR				; P1.1 (chave S1)
            bis.b	#BIT1,P1REN				; Configura como entrada
            bis.b	#BIT1,P1OUT				; Resistor de pull-up

            bic.b	#BIT1,P2DIR				; P2.1 (chave S2)
            bis.b	#BIT1,P2REN				; Configura como entrada
            bis.b	#BIT1,P2OUT				; Resistor de pull-up

main:
			mov.w	#0x123,R12

main_loop:
			call	#lfsr					; Gera numero aleatorio
			call 	#wait					; Espera um tempo aleatorio [128 ms a 4 sec]
			call	#display				; Faz piscar os LEDs 1 vez
			call	#readInput				; Aguarda o usuario responder
											; Mostra para o usuario se ele acertou ou nao
											; - Pode ter acertado
											; - Pode ter errado
											; - Acabou o tempo para responder

			jmp main_loop					; recomeça do inicio.
			nop

;-------------------------------------------------------------------------------
; Rotina readInput : faz a leitura dos botoes e remove o rebote
; Saida
; - R9: leitura do botao P1.1
; - R10: leitura do botao P2.1
readInput:
			; Complete aqui com o seu codigo
			mov.w	#0,R8					; Indicador de timeout
			mov.w	#0,R9					; Leitura do botao P1.1
			mov.w	#0,R10					; Leitura do botao P2.1
			mov.w	#0,R11					; Registrador MSB (16 bits)
			mov.w	#0,R4					; Registrador LSB (16 bits)

readInput_loop:
			inc		R4
			adc		R11

readInput_BT1:								; Verifica botao P1.1
			bit.b	#BIT1,P1IN
			jc		readInput_BT2

readInput_BT12:								; Verifica botao P2.1 (P1.1 ja pressionado)
			call	#debounce
			bit.b	#BIT1,P2IN
			jc		readInput_BT1_press

readInput_BT12_press:						; Botoes P1.1 e P2.1 pressionados
			mov.w	#1,R9					; Indica P1.1 pressionado
			mov.w	#1,R10					; Indica P2.1 pressionado
			jmp 	readInput_finished_BT12

readInput_BT1_press:						; Botao P1.1 pressionado
			call	#debounce
			mov.w	#1,R9					; Indica P1.1 pressionado
			jmp		readInput_finished_BT1

readInput_BT2:								; Verifica botao P2.1
			bit.b	#BIT1,P2IN
			jc		readInput_loop

readInput_BT2_press:						; Botao P2.1 pressionado
			call	#debounce
			mov.w	#1,R10					; Indica P2.1 pressionado
			jmp		readInput_finished_BT2

readInput_finished_BT12:					; Botoes P1.1 e P1.2 pressionados
			call	#timeout				; Verifica se existe timeout
			tst		R8						; Se existir
			jnz		readInput_ret			; Retorna
			tst		R7						; LED vermelho
			jz		readInput_endgame_lost
			tst		R6						; LED verde
			jz		readInput_endgame_lost
			jmp		readInput_endgame_win	; LED verde e vermelho acesos

readInput_finished_BT1:						; Botao P1.1 pressionado
			call	#timeout				; Verifica se existe timeout
			tst		R8						; Se existir
			jnz		readInput_ret			; Retorna
			tst		R6						; LED verde
			jz		readInput_endgame_lost	; LED verde apagado
			tst		R7						; LED vermelho
			jnz		readInput_endgame_lost	; LED vermelho aceso
			jmp		readInput_endgame_win

readInput_finished_BT2:						; Botao P2.1 pressionado
			call	#timeout				; Verifica se existe timeout
			tst		R8						; Se existir
			jnz		readInput_ret			; Retorna
			tst		R7						; LED vermelho
			jz		readInput_endgame_lost	; LED vermelho apagado
			tst		R6						; LED verde
			jnz		readInput_endgame_lost  ; LED verde aceso

readInput_endgame_win:
			call	#led2_pisca				; Pisca o LED verde
			call	#led2_pisca
			call	#led2_pisca
			jmp		readInput_ret

readInput_endgame_lost:
			call	#led1_pisca				; Pisca o LED vermelho
			call	#led1_pisca
			call	#led1_pisca

readInput_ret:
			ret

;-------------------------------------------------------------------------------
timeout:
			cmp.w	#2,R11					; Compara 0x20000 com R11 e R4
			jlo		timeout_not_exist
			mov.w	#1,R8					; Indica timeout

timeout_exist:
			call	#led12_pisca
			call	#led12_pisca
			call 	#led12_pisca
			ret

timeout_not_exist:
			mov.w	#0,R8					; Nao houve timeout
			ret

;-------------------------------------------------------------------------------
led12_pisca:								; Pisca LEDs verde e vermelho
			xor.b	#BIT0,P1OUT
			xor.b	#BIT7,P4OUT
			call	#delay
			xor.b	#BIT0,P1OUT
			xor.b	#BIT7,P4OUT
			call	#delay
			ret

led1_pisca:									; Pisca LED vermelho
			xor.b	#BIT0,P1OUT
			call	#delay
			xor.b	#BIT0,P1OUT
			call	#delay
			ret

led2_pisca:									; Pisca LED verde
			xor.b	#BIT7,P4OUT
			call	#delay
			xor.b	#BIT7,P4OUT
			call	#delay
			ret

;-------------------------------------------------------------------------------
debounce:
			push	R4
			mov		#5000,R4

debounce_loop:
			dec 	R4
			jnz		debounce_loop

debounce_ret:
			pop		R4
			ret

;-------------------------------------------------------------------------------
; Rotina display : Acende os LEDs com base no conteúdo de R12
; Entradas
; - R12: Número aleatorio gerado por lfsr
; Saidas
; - LEDs: Acende de acordo com BIT9 (Verde) e BIT1 (Vermelho)

display:
			; Complete aqui com o seu codigo
			mov.w 	#0,R5

display_green:
			mov.w	#0x200,R6				; LED verde (BIT9)
			and		R12,R6
			jz		display_red

display_led_green:
			bis.b	#BIT7,P4OUT				; Liga o LED verde
			mov.w 	#1,R5

display_red:
			mov.w	#0x2,R7					; LED vermelho (BIT1)
			and		R12,R7
			jz		display_desliga_led

display_led_red:
			bis.b	#BIT0,P1OUT				; Liga o LED vermelho
			mov.w 	#1,R5

display_desliga_led:
			call	#delay
			bic.b	#BIT7,P4OUT
			bic.b	#BIT0,P1OUT

display_test:								; Testa se algum dos LEDs esta ligado
			tst.b	R5
			jnz		display_ret

display_no_led:
			call	#lfsr					; Gera outro numero aleatorio
			jmp		display					; Display nos LEDs

display_ret:
			ret

;-------------------------------------------------------------------------------

delay:										; Atraso do LED
			push	R4
			mov		#50000,R4

delay_loop:
			dec 	R4
			jnz		delay_loop

delay_ret:
			pop		R4
			ret

;-------------------------------------------------------------------------------
; Rotina wait : Espera um tempo dado em ms
; Entradas
; - R12: valor de tempo em ms
wait:
			; Complete aqui com o seu codigo
			push	R12						; Guarda contexto
			rra.w	R12						; Divide por 128
			rra.w	R12
			rra.w	R12
			rra.w	R12
			rra.w	R12
			rra.w	R12
			rra.w	R12

wait_loop:
			dec		R12
			jz		wait_ret
			mov		#44740,R11				; Batidas de clk em 128ms
											; 1clk	-	1/2^20
											; 3xclk 	-	0,128

wait_delay:
			dec 	R11
			jnz		wait_delay
			jz		wait_loop

wait_ret:
			pop		R12						; Recupera contexto
			ret

;-------------------------------------------------------------------------------
; Rotina lfsr: Linear Feedback Shift Register
; Saida
; - R12: Numero pseudo-aleatorio
lfsr:
			; Complete com o seu codigo aqui
			rra.w	R12
			jc		lfsr_xor
			jmp 	lfsr

lfsr_xor:
			add.w	#0x400,R12				; Bit 11 recebe 1
			mov.w	#0xCA0,R6				; Mascara com bits 11, 10, 7, 5 setados
			xor.w	R12,R6
			mov.w	R6,R12

lfsr_ret:
			ret
;-------------------------------------------------------------------------------
; RAM
;-------------------------------------------------------------------------------
			.data

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

