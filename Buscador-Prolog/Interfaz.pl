[library(pce)].
use_modele(library(pce)).

%ejecuta el proceso que se elija, tipo de busqueda
ejecutar(Tipo,Param1,Param2,Param3,Param4,Param5,Param6):-
	(Tipo==ficha,
		buscar(Param1,T,A,_X,_Y,_Z),
		msgbox(T,A,Param1);
	(Tipo==titulo,
		buscar(F,Param2,A,_X,_Y,_Z),
		msgbox(Param2,A,F);
	(Tipo==autor,
		buscar(F,T,Param3,_X,_Y,_Z),
		msgbox(T,Param3,F);
	(Tipo==clasificacion,
		buscar(F,T,A,Param4,_X,_Y),
		msgbox(T,A,F);
	(Tipo==anio,
		buscar(F,T,A,_Y,Param5,_X),
		msgbox(T,A,F);
	%solo en la busqueda libre escribe una oración completa ahi la procesamos con ambos parametros
	(Tipo==libre,
		RESULTADO=Param6,
		write('Busqueda Libre'),
		%msgbox(RESULTADO),
		igrama(RESULTADO,Z),
		atomic_list_concat(Z,' ',RESULTADOf),
		write(RESULTADOf),
		busquedaAvanzada(Z,T,A,F),
		msgbox(T,A,F),
				%msgbox(RESULTADOf,RESULTADOf,RESULTADOf),
		write('finalizo');!)))))).

%llama a la funcion oración para desmenbrar la oración y obtener el tipo de busqueda a realizar
igrama(RESULTADO,Z):-
	atomic_list_concat(L,' ',RESULTADO),
	oracion(L,Z).


%da la ventana de salida a lo obtenido en ejecutar
msgbox(Libro,Autor,Ficha):-
	new(ND,dialog("Resultado", size(600,300))),
	new(LIBRO,text_item("Libro:")),
	send(LIBRO,selection,Libro),
	new(AUTOR,text_item("Autor:")),
	send(AUTOR,selection,Autor),
	new(FICHA,text_item("Ficha:")),
	send(FICHA,selection,Ficha),
	new(@boton, button("Cerrar",
	and(
		message(ND, destroy),
		message(ND, free),
		message(@boton, free)))),
	send(ND, append(LIBRO)),
	send(ND, append(AUTOR)),
	send(ND, append(FICHA)),
	send(ND, append(@boton)),
	send(ND,open).

%principal función del programa
constructor:-
	%new(F, frame("visor de procesos")),
	new(D,dialog("Busqueda")),
	new(T1,text_item("Ficha")),
	new(T2,text_item("Titulo")),
	new(T3,text_item("Autor")),
	new(T4,text_item("Clasificación")),
	new(T5,text_item("Año")),
	new(T6,text_item("Lenguaje Natural")),

	%send(D,append(new(L, label(nombre, "procesos"))),
	send(D, append(new(B, browser))),
	send_list(B, append,[ficha,titulo,autor,clasificacion,anio,libre]),

	new(EJE,button(procesar,
		and(
			message(@prolog,ejecutar,
				B?selection?key,
				T1?selection,
				T2?selection,
				T3?selection,
				T4?selection,
				T5?selection,
				T6?selection)))),
	%send(D,append(F)),
	send(D,append(T1)),
	send(D,append(T2)),
	send(D,append(T3)),
	send(D,append(T4)),
	send(D,append(T5)),
	send(D,append(T6)),
	send(D,append(EJE)),
	send(D,open).

typeB(X,[X,X]).

inicio:-
	consult('gramatica.pl'),
	consult('Busquedas.pl'),
	constructor.

