.model small
extrn reto:near
extrn lee1:near
extrn dir:near
.386
.stack
.data

;Para las funciones dir
buf	label byte	; Lista de parametros del nombre:
max_blen	db	80	;  		* long max del nombre
bufer_len	db	?	;  		* #n de caracteres introducidos
bufer		db	81	DUP(' ')	;  * nuevo nombre del fichero

;Para el renombramiento
nuevopar	label byte	; Lista de parametros del nombre:
max_nlen	db	80	;  		* long max del nombre
nuevo_len	db	?	;  		* #n de caracteres introducidos
nuevo		db	81	DUP(' ')	;  * nuevo nombre del fichero

viejopar	label byte	; Lista de parametros del nombre:
max_vlen	db	80	;  		* long max del nombre
viejo_len	db	?	;  		*# n de caracteres introducidos
viejo		db	81	DUP(' ')	;  * nombre original del fichero

.code

;Macros usadas en el programa
print macro cadena
local	dbcad,dbfin,salta
	pusha			;respalda todo ax,bx,cx,dx
	push ds			;respalda DS, porque vamos a usar segmento de código
	mov dx,cs		;segmento de codigo será también de datos
	mov ds,dx		
	mov dx,offset dbcad 	;direccion de cadena (en segmento de código)
	mov ah,09h		
	int 21h			;desplegar
	jmp salta		;saltar datos para que no sean ejecutados
	dbcad db cadena 	;aqui estara la cadena pasada en la sustitucion
	dbfin db 24h		;fin de cadena
salta:	pop ds			;etiqueta local de salto, recuperar segmento de datos
	popa			;recuperar registros, opuesto a pusha
endm	print


main:	
	mov ax,@data
	mov ds,ax
	mov es,ax
	
	;Bucle principal
  buclePri:	
  	;Menú del gestor
  	print "*************************************************************************"
  	call reto
	print "*	Gestor de archivos						*"
	call reto
	print "* 0. Dir	: Despliega el contenido del directorio actual		*"
	call reto
	print "* 1. Cd	: Para regresar al directorio raiz ----> '/'			*"
	call reto
	print "* 2. Crear subdirectorio	5. Crear archivo			*" 
	call reto
	print "* 3. Eliminar subdirectorio	6. Eliminar archivo			*"
	call reto
	print "* 4. Renombrar subdirectorio	7. Renombrar archivo			*"
	call reto
	print "* 				8. Salir				*"
  	call reto
	print "*************************************************************************"
  	call reto
	
	;Lectura de la opción escogida
	call lee1	;resultado en la parte baja del registro ax, [al]
	call reto
	
	;Comparación para definir lo que debe realizar el programa
	
	;Primero lo de los directorios
	cmp al,0	; Dir, desplegar el contenido actual del directorio
	je dir_ac	; directorio actual
	
	cmp al,1	; Cd, para entrar en un subdirectorio
	je cd_cha	; cd cambiar directorio
	
	cmp al,2	; Para crear subdirectorio
	je crea_dir	; 
	
	cmp al,3	; Para borrar un directorio
	je borrar_dir
	
	cmp al,4	; Para renombrar un directorio
	je renombra_dir
	
	;Ahora los archivos
	cmp al,5	; Para crear un archivo
	je crea_fic
	
	cmp al,6	; Para borrar un archivo
	je borrar_fic
	
	cmp al,7	; Para renombrar un archivo
	je renombrar_fic
	
	cmp al,8	; Para salir del programa
	je out1
       jmp buclePri	
out1:
	.exit 0
	

	;Para los directorios las funciones y saltos

	;----------------------------------------------------------------------------
	
  dir_ac:
	call dir	;llamo a la función dir de fun
	call reto	
       jmp buclePri	; regresamos a el inicio del gestor
	
  cd_cha:
  	print "Introdusca el nombre del subdirectorio: "
  	call leecad
  	mov dx,offset bufer
  	mov ah,3bh
  	int 21h
  	jc error_cd
       jmp buclePri
       
  error_cd:
  	call reto
	print "Ocurrio un error al acceder al directorio."
	call reto
	jmp buclePri
       
        ;----------------------------------------------------------------------------
        
  crea_dir:
	print "Introdusca el nombre del directorio a crear: "
	call leecad		;en dx se encuentra tamb
	mov dx,offset bufer
	mov ah, 39h 
	int 21h
	jc error_1
       jmp buclePri
       
error_1:
	call reto
	print "Ocurrio un error al crear el directorio."
	call reto
	jmp buclePri
	
	
  borrar_dir: 
	print "Introdusca el nombre del directorio a borrar: "
	call leecad	;el nombre en el bufer
	mov dx,offset bufer
	mov ah,3Ah 
	int 21h
	jc error_2
       jmp buclePri
       
error_2:
	call reto
	print "Error al eliminar el subdirectorio."
	call reto
	jmp buclePri
	
  renombra_dir:
  	print "Introduce el nombre del directorio a renombrar: "
  	call cadNombreA	
  	call delimitarNA
  	call reto
	
  	cmp viejo_len,00
	je renombra_dir
	
	print "Introduce el nuevo nombre del directorio: "
	call cadNombre		;cadena del nombre nuevo
  	call delimitarN
  	call reto
	
  	cmp nuevo_len,00         ;si no se escribe ningun nombre
  	je renombra_dir

        mov ah,56h             ;Funci¢n que renombra o mueve
        mov dx,offset viejo            ;Directorio o fiche a renombrar
        mov di,offset nuevo            ;Renombra el dir o fich con la nueva cadena
        int 21h                 ;introducida por teclado.
        jc error_3
        jmp buclePri
        
error_3:
	call reto
	print "Se produjo un error al renombrar."
	call reto
	jmp buclePri
	
	;
	
  leecad:
  	mov ah,0Ah
        mov dx,offset buf         ;recoge la cadena a renombrar, desde teclado
        int 21h
        
        mov bh,00
        mov bl,bufer_len	;Delimita cadena introducida a tamaño namelen
        mov bufer[bx],00h	;Termina cadena con 00h para poder renombrar 
        mov bufer[bx+1],'$'	;Pone delimitador de exhibici¢n de caracteres
        ret
	
	;Para renombrar un directorio
  cadNombre:
        mov ah,0Ah
        mov dx,offset nuevopar         ;recoge la cadena a renombrar, desde teclado
        int 21h
        ret
        
  cadNombreA:
        mov ah,0Ah
        mov dx,offset viejopar         ;recoge cadena original desde teclado
        int 21h
        ret
        
  delimitarN:
        mov bh,00
        mov bl,nuevo_len	;Delimita cadena introducida a tamaño namelen
        mov nuevo[bx],00h	;Termina cadena con 00h para poder renombrar 
        mov nuevo[bx+1],'$'	;Pone delimitador de exhibici¢n de caracteres
        ret
        
  delimitarNA:
        mov bh,00
        mov bl,viejo_len	;Delimita cadena introducida a tamaño viejo_len
        mov viejo[bx],00h	;Termina cadena con 00h para poder renombrar 
        mov viejo[bx+1],'$'	;Pone delimitador de exhibici¢n de caracteres
        ret
        
        ;----------------------------------------------------------------------------
	;Para archivos se tiene lo siguiente
	
	;En la creacion de los archivos se define la extension como parte del nombre
  crea_fic:
  	print "Introduce el nombre del archivo: "
	call cadNombre		;cadena del nombre nuevo
  	call delimitarN
  	call reto
  	
  	cmp nuevo_len,00         ;si no se escribe ningun nombre se retorna al ciclo
  	je error_4
  	;crear
	mov ah,3ch
	mov cx,0
	mov dx,offset nuevo
	int 21h
	jc error_4	;si no se pudo crear
	call reto
        print " Fichero creado con éxito."
	call reto
	mov bx,ax
	mov ah,3eh ;cierra el archivo
	int 21h
	jc error_4
	jmp buclePri
	
error_4:
	call reto
	print "Se produjo un error al crear el archivo."
	call reto
	jmp buclePri
	
	
	;Para borrar un archivo se tiene que escribir la extension como parte del nombre
  borrar_fic:
  	print "Introduce el nombre del archvio a borrar: " 
  	call cadNombre		;cadena del nombre dell archivo a borrar
  	call delimitarN
  	call reto
  	cmp nuevo_len,00        ;si no se escribe ningun nombre se retorna al ciclo
  	je error_5
  	mov ah,41h 
	mov dx, offset nuevo
	int 21h  
	jc error_5		;Si hubo error
	call reto
	print "Archivo eliminado con exito"
	call reto
	jmp buclePri
	
error_5:
	call reto
	print "Se produjo un error al eliminar el archivo."
	call reto
	jmp buclePri
	
  renombrar_fic:
  	print "Introduce el nombre del archivo a renombrar: "
  	call cadNombreA	
  	call delimitarNA
  	call reto
	
  	cmp viejo_len,00
	je renombrar_fic
	
	print "Introduce el nuevo nombre del archivo: "
	call cadNombre				;cadena del nombre nuevo
  	call delimitarN
  	call reto
	
  	cmp nuevo_len,00			;si no se escribe ningun nombre
  	je renombrar_fic
  	
  	mov ah,56h				;Funcion que renombrao mueve
        mov dx,offset viejo			;Directorio o fiche a renombrar
        mov di,offset nuevo			;Renombra el dir o fich con la nueva cadena
        int 21h 				;introducida por teclado.
        jc error_6
        jmp buclePri
        
error_6:
	call reto
	print "Se produjo un error al renombrar."
	call reto
	jmp buclePri

	
;Para la creacion/renombramiento/eliminacion de archivos se debe incluir la extensiones de los mismos.
;El tamaño maximo es de 8 letras por nombre.
;

;Proyecto Final
;Realizar un Gestor de archivos
;Debe realizar lo siguiente:
;Navegar por directorios
;Borrar archivos		
;Copiar archivos
;Mover  archivos

end 