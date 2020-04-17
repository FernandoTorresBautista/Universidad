<!DOCTYPE html>
<html>
    <head>
        <meta charset="UTF-8">
        <title>Knn</title>
        <style> 
            header { width: 100%; height: 40px; background: #FFFFFF; font-size: 2em; text-align: center; clear: both; } 
            section { position: relative; } 
            #izquierda { position: relative; width: 40%; top: 0; left: 0; background: #FFFFFF; float: left;  } 
            #derecha { position: relative; width: 60%; top: 0; right: 0; background: #FFFFFF; float: right;  } 
        </style> 
        <script type="text/javascript">
            var matriz_puntos = Array();
            var matriz_puntosp = Array();
            var m_claseobtenida = Array();
            var numero_ejemplo = 0, k, datos = "", ejemplo_salida = "", puntos_salida="", myVar;
            function clase(n){
                if(n>=0){
                    return 1;
                }
                return 0;
            }
            function cuadricula(ctx){
                for (var x=0; x<=600; x=x+12){
                    ctx.moveTo(x,0);
                    ctx.lineTo(x,600);
                }
                for (var y=0; y<=600; y=y+12){
                    ctx.moveTo(0,y);
                    ctx.lineTo(600,y);
                }
                ctx.strokeStyle = "#00ffff";
                ctx.stroke();
            }
            function dibujarPuntos(ctx){
                for ( var x2 in matriz_puntos ){
                    ctx.beginPath();
                    var x        = matriz_puntos[x2][0]*12;               //pos x
                    var y        = 600 - matriz_puntos[x2][1]*12;         //pos y
                    var radio    = 6;                                     // radio del arco
                    ctx.arc(x,y,radio,0,2*Math.PI, true);
                    ctx.fillStyle = "#0000ff";
                    ctx.strokeStyle = "#0000ff";
                    if( matriz_puntos[x2][2]>=0 ){
                        ctx.fill();
                    }
                    else{
                        ctx.stroke();
                    }
                }
            }
            function claseobtenida(distancias){
                var uno=0;
                var muno = 0;
                for(var j=0;j<distancias.length;j++){
                    if( matriz_puntos[distancias[j][1]][2]>=0 ){
                        uno++;
                    }
                    else{
                        muno++;
                    }
                }
                if(uno>=muno){
                    puntos_salida += (numero_ejemplo+1) +"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"+ 1 +"<br>";
                    return 1;
                }
                else{
                    puntos_salida += (numero_ejemplo+1)+ "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"+ (-1) +"<br>";
                    return -1;
                }
            }
            function compareNumber(a,b) {
                return a[0]-b[0];
            }
            function distancias(){
                var distancias = Array();
                datos  = "Distancias:<br><br>"
                for(var i=0;i<matriz_puntos.length;i++){
                    distancias[i] = Array();
                    distancias[i][1]=i;
                    var dis = Math.sqrt(Math.pow(Math.abs(matriz_puntosp[numero_ejemplo][0]-matriz_puntos[i][0]),2)
                            + Math.pow(Math.abs(matriz_puntosp[numero_ejemplo][1]-matriz_puntos[i][1]),2));
                    distancias[i][0] = dis;
                    datos = datos + "d("+(numero_ejemplo+1)+","+(i+1)+") = "+dis+"<br>";
                }
                distancias.sort(compareNumber);
                distancias = distancias.slice(0,k);
                var clasen = claseobtenida(distancias);
                ejemplo_salida = "Ejemplo de prueba: "+(numero_ejemplo+1)+"<br>Clase calculada: "+ clasen +"<br>";
                ejemplo_salida += "Punto( "+matriz_puntosp[numero_ejemplo][0]+" , "+matriz_puntosp[numero_ejemplo][1] +")"
                return distancias;
            }
            function iterapuntos(ctx){
                var kdistancias = distancias();
                for(var j=0;j<kdistancias.length;j++){
                    var x = matriz_puntos[kdistancias[j][1]][0]*12;
                    var y = 600 - matriz_puntos[kdistancias[j][1]][1]*12;
                    ctx.beginPath();
                    ctx.arc(x,y,4,0,2*Math.PI, true);
                    ctx.strokeStyle = "#ff00ff";
                    ctx.fillStyle = "#ff00ff";
                    ctx.stroke();
                    ctx.fill();
                }
                ctx.beginPath();
                var x        = matriz_puntosp[numero_ejemplo][0]*12;
                var y        = 600 - matriz_puntosp[numero_ejemplo][1]*12;
                ctx.arc(x,y,4,0,2*Math.PI, true);
                ctx.strokeStyle = "#ff0000";
                ctx.fillStyle = "#ff0000";
                if( matriz_puntos[numero_ejemplo][2]>=0){
                    ctx.stroke();
                }
                else{
                    ctx.fill();
                }
                ctx.beginPath();
                var radio = kdistancias[2][0]*12;
                ctx.arc(x,y,radio,0,2*Math.PI, true);
                ctx.strokeStyle = "#000000";
                ctx.stroke();
            }
            function animacion(ctx){
                k = parseInt(document.getElementById('valork').value);
                ctx.clearRect(0,0,600,600);
                cuadricula(ctx);
                dibujarPuntos(ctx);
                iterapuntos(ctx);
                document.getElementById('ejemplo').innerHTML = ejemplo_salida;
                document.getElementById('datos').innerHTML = datos;
                numero_ejemplo++;
                if( numero_ejemplo == matriz_puntosp.length ){
                    document.getElementById('ejemplo1').innerHTML = puntos_salida;
                    myStopFunction();
                }
            }
            function myStopFunction() {
                clearInterval(myVar);
            }
            function processFiles(files){
                var archivo = files[0];
                if ( archivo ) {
                    var reader = new FileReader();
                    reader.onload = function(e) {
                        var contents = e.target.result;
                        var linea = contents.split("\n");
                        for(var i=0;i<linea.length;i++){
                            matriz_puntos[i] = Array();//linea es un array de numero
                            matriz_puntos[i] = linea[i].split(" ");//para cada linea separar los numeros
                        }
                        var numero_ejemplos = parseInt(matriz_puntos[0][0]);
                        var matriz_p = matriz_puntos.slice(1,numero_ejemplos+1);
                        var numero_pruebas = matriz_puntos[numero_ejemplos+1][0];
                        matriz_puntosp = matriz_puntos.slice( numero_ejemplos+2,numero_ejemplos+numero_pruebas+2);
                        matriz_puntos = matriz_p;
                        puntos_salida += "Punto&nbsp;&nbsp;&nbsp;Clase asignada<br>";
                        var canvas = document.getElementById('canvas');
                        var ctx = canvas.getContext("2d");
                        animacion(ctx);
                        myVar = setInterval(function(){animacion(ctx)}, 2000);
                    }
                    reader.readAsText(archivo);
                }
            }
            document.getElementById('fileinput').addEventListener('change', readSingleFile, false);
        </script>
    </head>
    
    <body >
        <section> 
            <div id="izquierda">
                <center>
                    Antes de cargar el archivo, introducir el valor de K<br><br>
                    K:
                    <select id="valork" name="option">
                        <option value="3">3</option>
                        <option value="5">5</option>
                        <option value="7">7</option>
                    </select>
                    <input type="file" id="files" size="50" onchange="processFiles(this.files)" ><br><br>
                    <p id="ejemplo" ></p><br><br>
                    <p id="datos" ></p>
                </center>
            </div>
        </section>
        <section> 
            <div id="izquierda">
                <canvas id="canvas" height="600px" width="600px" ></canvas><br>
                <p id="ejemplo1" ></p>
            </div>
        </section>
    </body>
</html>
