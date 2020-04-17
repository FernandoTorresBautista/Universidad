<!DOCTYPE html>
<html>
<head>
	<meta charset="UTF-8">
        <title>Cross Validation</title>
        <style> 
            header { width: 100%; height: 40px; background: #FFFFFF; font-size: 2em; text-align: center; clear: both; } 
            section { position: relative; } 
            #izquierda { position: relative; width: 50%; top: 0; left: 0; background: #FFFFFF; float: left;  } 
            #derecha { position: relative; width: 50%; top: 0; right: 0; background: #FFFFFF; float: right;  } 
        </style> 
	<script type="text/javascript">
            var entrenamiento = new Array();
            var contenido = new Array();
            var matriz_k = new Array();
            var pruebas = new Array();
            var ejemplos_c = new Array();
            var pos = new Array();
            var ejemplos;
            var clases;
            function salida() {
                var salida = "Datos de las 10 iteraciones <br>";
                for (var i = 0; i < 10; i++) {
                    salida += "K"+(i+1)+"={";
                    for (var l = 0; l < matriz_k[i].length; l++) {
                        salida += " "+matriz_k[i][l];
                    }
                    salida += "}<br><br>";
		}
                document.getElementById("salida").innerHTML= salida + "<br>"+ pos;
            }
            
            function obtener_datos(linea){
                var cont=0;
                for(var i=0;i<linea.length;i++){
                    if (i == 0){
                        var n = linea[i].split(" ");
                        //numero de ejemplos y clases
                        ejemplos=parseInt(n[0]);
                        clases=parseInt(n[1]);
                    }
                    else{
                        //ejemplos de clases
                        ejemplos_c[cont] = parseInt(linea[i]);
                        cont++;
                    }
                }
                document.getElementById("ejemplo").innerHTML = "Ejemplos: " + ejemplos + "<br>Clases:" + clases +"<br>";
                cont = 0;
                //80% para entrenamiento y 20% para pruebas
                for (var i = 0; i <ejemplos_c.length; i++) {
                    if (((ejemplos_c[i]*0.8)%1)==0) {
                        entrenamiento[cont] = parseInt((ejemplos_c[i]*0.8));
                        pruebas[cont] = parseInt(ejemplos_c[i]-(ejemplos_c[i]*0.8));
                        cont++;
                    }
                    else{		
                        var pE = parseInt( (ejemplos_c[i]*0.8) );
                        var pP = parseInt( ejemplos_c[i]-pE );
                        entrenamiento[cont] = pE;
                        pruebas[cont] = pP;
                        cont++;
                    }
                }
                document.getElementById("ejemplo1").innerHTML = "Entrenamiento: " + entrenamiento + "<br>Pruebas:" + pruebas +"<br>";
                cont=1;
                var posicion=0;
                //Empezamos en la posicion 0
                pos[0]=Number(posicion);
                for (var i = 0; i < ejemplos_c.length-1; i++) {
                    //y para llegar a la suma del 80%
                    posicion = ejemplos_c[i] + posicion;
                    pos[cont] = posicion;
                    cont++;
                }
                //modulo de 10 de cada subíndice y se guarda 
                for( var i = 0; i < 10 ; i++ ){
                    contenido[i]=new Array();
                    for (var j = 0; j < entrenamiento[i]; j++) {
                        contenido[i][j] = (j%10);
                    }
                }
                for (var l = 0; l < 10; l++) {
                    matriz_k[ l ]=new Array();
                    var ejemp=0;
                    for (var i = 0; i < contenido.length; i++) {
                        for (var j = 0; j < contenido[i].length; j++) {
                            if ( contenido[i][j]==l ) {
                                matriz_k[l][ejemp] = pos[i]+j;
                                ejemp++;
                            }
                        }
                    }
                }
            }
            
            function processFiles(files){
                var file=files[0];
                if (file){
                    var reader = new FileReader();
                    reader.onload=function(e) {
                        var contents = e.target.result;
                        //array de lineas
                        var linea = contents.split("\n");
                        obtener_datos(linea);
                        salida();
                    }
                    reader.readAsText(file);
                }
            }
    </script>
    <body >
        <section> 
            <div id="izquierda">
                <center>
                    <form align="center">
                        <input id="archivo"  type="file"  size="50" onchange="processFiles(this.files)"><br>
                    </form>
                    <p id="ejemplo"> </p>
                    <p id="ejemplo1"> </p>
                    <p id="salida"> </p>
                </center>
            </div>
        </section>
    </body>
</html>