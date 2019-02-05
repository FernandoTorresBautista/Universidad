.model small
public dir
public despc
public reto
public lee1
.stack
.data

ndir	db	164 dup(0)	;obtener el nombre del directorio
comod	db	"*.*",0		;comodin
DTA	db	21 dup(0)
attr	db	0
time	dw	0
date	dw	0
sizel	dw	0
sizeh	dw	0
fname	db	13 dup(0)


.code

lee1:   mov ah,01h
        int 21h
        sub al,30h
        cmp al,09h
        jle sl1
        sub al,07h
        cmp al,0Fh
        jle sl1
        sub al,20h
 sl1:   ret

reto:   push ax
        push dx
        mov ah,02h
        mov dl,0Dh
        int 21h
        mov dl,0Ah
        int 21h
        pop dx
        pop ax
        ret
	
	
dir:
	push ax
	push bx
	push cx
	
	mov dl,0		;Unidad actual
	mov si,offset ndir	;ds:si buffer
	mov Ah,47h		;leer directorio actual
	int 21h
	jc error
			
	;desplegar directorio acual
	push offset ndir
	call despc
	add sp,02
	call reto
	
	;establecer posicion de DTA
	mov ah,1Ah
	mov dx,offset DTA
	int 21h
	
	;preparar estructura del directorio y mostrar primer 
	mov dx,offset comod
	mov cx,10h
	mov ah,4Eh	;se pasa el comodin y busca el primero
	int 21h
	
	push offset fname
	call despc
	add sp,02
	call reto
	;mostrar el resto de los archivos
nf:	mov ah,4Fh
	int 21h
	jc salida
	push offset fname
	call despc
	add sp,02
	call reto
	jmp nf
	
error:

salida:
	pop cx
	pop bx
	pop ax
	ret

		
despc:	push bp
	mov bp,sp
	mov ah,02h
	cld
	mov si,[bp+4]
dcl:	lodsb		;carga en AL, incrementa SI
	cmp al,0
	je dcs
	mov dl,al
	int 21h
	jmp dcl
dcs:	pop bp
	ret
end
