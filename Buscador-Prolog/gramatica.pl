
%por favor muestrame libros de [tema]
%por favor por favor quiero que me muestres libros de [tema]
%por favor buscame libros acerca de [tema]
%por favor muestrame libros del autor [autor]
%por favor muestrame el libro [libro] 
%por favor busca el autor del libro [libro]
%necesito que me muestres libros del año [ano]
%necesito informacion sobre [tema]
%podrias mostrarme informacion acerca de [tema]
%podrias encontrar informacion sobre [tema]
%existe un libro con ficha [ficha]
%existe algun libro del autor [autor]
%muestrame libros acerca de [tema]
%ayudame a encontrar libros del autor [autor]

inicio(Z):-
	write('Que desea: '),
	read(Mi_Query),
	atomic_list_concat(L,' ', Mi_Query),
	oracion(L,Z).

oracion --> inicio, proposicion, fin.

fin --> nombre, preposicion.
fin --> nombre, preposicion, articulo, nombre.
fin --> nombre, contraccion, nombre.
fin --> cuantificador, nombre, preposicion.
fin --> articulo, nombre.

proposicion --> sintagma, proposicion.
proposicion --> sintagma, sintagma.
proposicion --> conjuncion, sintagma.
proposicion --> sintagma, nombre, sintagma.
%proposicion --> preposicion, nombre.
proposicion --> cuantificador, sintagma.
proposicion --> [].

sintagma --> verbo, preposicion, verbo.
sintagma --> verbo, proposicion.
sintagma --> articulo, nombre, proposicion.
sintagma --> personal, verbo.
sintagma --> verbo.
sintagma --> relativo.
sintagma --> articulo.
sintagma --> verbo, articulo.
sintagma --> verbo, cuantificador.

inicio --> [por],[favor].
inicio --> [necesito].
inicio --> [podrias].
inicio --> [existe].
inicio --> [muestrame].
inicio --> [ayudame].

nombre --> [libro];[libros];[informacion];[autor];[ficha];[anio].

preposicion --> [de];[sobre];[sobre],[el];[a];[con];[del].
preposicion --> [acerca],[de];[que],[hablen],preposicion.

%busqueda --> [tema];[autor];[titulo].
personal --> [me].

verbo --> [busca];[muestres];[quiero];[dame];[tengan];[solicito];[mostrarme];
          [busco];[podrias];[darme];[necesito];[ayudame];[decirme];[encontrar];
          [encontrar];[necesito];[muestrame];[buscame];[saber].

verbo --> [estoy], [buscando].

conjuncion --> [que].
articulo --> [los];[el];[del];[a].
relativo --> [que].

cuantificador --> [algunos];[un];[todos];[algun].
contraccion --> [del].






%phrase(oracion,[por,favor,quiero,que,me,muestres,libros,de,tema],Z). 	tema	
%phrase(oracion,[por,favor,buscame,libros,acerca,de,tema],Z).			tema
%phrase(oracion,[por,favor,muestrame,libros,del,autor,autor],Z).		autor, [autor]
%phrase(oracion,[por,favor,muestrame,informacion,del,libro,libro],Z).	libro [libro]
%phrase(oracion,[por,favor,busca,el,autor,del,libro,libro],Z).			libro [libro]
%phrase(oracion,[necesito,informacion,sobre,tema],Z).					[tema]
%phrase(oracion,[necesito,que,me,muestres,libros,del,año,año],Z).		año,[año]
%phrase(oracion,[podrias,encontrar,informacion,sobre,tema],Z).			[tema]
%phrase(oracion,[podrias,mostrarme,informacion,acerca,de,tema],Z).		[tema]
%phrase(oracion,[podrias,mostrarme,libros,del,autor,autor],Z).			autor,[autor]	
%phrase(oracion,[muestrame,libros,acerca,de,tema],Z).					[tema]
%phrase(oracion,[muestrame,libros,del,año,año],Z).						año,[año]
%phrase(oracion,[existe,algun,libro,con,ficha,ficha],Z).				ficha, [ficha]
%phrase(oracion,[existe,algun,libro,del,autor,autor],Z).				autor, [autor]
%phrase(oracion,[ayudame,a,encontrar,libros,del,autor,autor],Z).		autor, [autor]