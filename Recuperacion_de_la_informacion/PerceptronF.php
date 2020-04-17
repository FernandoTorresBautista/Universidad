<!DOCTYPE html>
<html>
    <head>
        <meta charset="UTF-8">
        <title>Perceptron</title>
        <style> 
        header { width: 100%; height: 40px; background: #FFFFFF; font-size: 2em; text-align: center; clear: both; } 
        section { position: relative; } 
        #izquierda { position: relative; width: 50%; top: 0; left: 0; background: #FFFFFF; float: left;  } 
        #derecha { position: relative; width: 50%; top: 0; right: 0; background: #FFFFFF; float: right;  } 
        </style> 
        <script type="text/javascript">
            function signo(n){
                if( n>=0 ){
                    return 1;
                }
                return -1;
            }
            function clase(n){
                if(n>=0){
                    return 1;
                }
                return 0;
            }
            function cuadricula(canvas){
                var ctx = canvas.getContext("2d");
                for (var x=0; x<=400; x=x+20){
                    ctx.moveTo(x,0);
                    ctx.lineTo(x,400);
                }
                for (var y=0; y<=400; y=y+20){
                    ctx.moveTo(0,y);
                    ctx.lineTo(400,y);
                }
                ctx.strokeStyle = "#00ffff";
                ctx.stroke();
                ctx.strokeStyle = "#0000ff";
            }
            function obtenerDatos(res, b, Y, nejemplo){
                for (var dato in res) {//cada linea despues de la cero son valores de ejemplos
                    if( dato==0 ){
                        var linea = res[dato].split(",");
                        entradas=linea[0];
                        var i;
                        for(i in linea){;}
                        ejemplos = linea[1];
                    }
                    else{
                        //guardar datos para bial, x1, x2, .., xn, salida
                        var linea = res[dato].split(",");
                        b[dato] = linea[0];
                        var i;
                        nejemplo[dato] = new Array();
                        for(i in linea){
                            nejemplo[dato][i]=linea[i];
                        }
                        Y[dato]=linea[i];
                    }
                }
            }
            function dibujarPuntos(canvas, nejemplo){
                var ctx = canvas.getContext("2d");
                var Puntos="<table><tr><td>Puntos Graficados: </td><td> Clase </td><tr>";
                for (var x2 in nejemplo){
                    ctx.beginPath();
                    var x        = nejemplo[x2][1]*20;                    // coordenada x 
                    var y        = 400 - nejemplo[x2][2]*20;              // coordenada y 
                    var radio    = 5;                                     // radio del arco
                    ctx.arc(x,y,radio,0,2*Math.PI, true);
                    if( clase(nejemplo[x2][3]) ){
                        Puntos = Puntos + "<tr><td>"+nejemplo[x2][1]+","+nejemplo[x2][2]+"</td><td>1</td></tr>";
                        ctx.fillStyle = "#0000ff";
                        ctx.stroke();
                    }
                    else{
                        Puntos = Puntos + "<tr><td>"+nejemplo[x2][1]+","+nejemplo[x2][2]+"</td><td>-1</td></tr>";
                        ctx.strokeStyle = "#0000ff";
                        ctx.fill();
                    }
                }
                document.getElementById("myid3").innerHTML = Puntos; 
            }
            function dibujalinea(canvas,yini,yfin){
                var ctx = canvas.getContext("2d");
                ctx.moveTo(-100,400-yini);
                ctx.lineTo(400,400-yfin);
                ctx.stroke();
            }
            function calcularPesos(final, nejemplo, pesos_t, Y){
                for(var x in nejemplo){
                    var salidacalculada=0;
                    salidacalculada =  pesos_t[1]*nejemplo[x][0] + pesos_t[2]*nejemplo[x][1] + pesos_t[3]*nejemplo[x][2];
                    var Ycalculada = signo(salidacalculada);
                    var Salidadiferencia = Y[x] - Ycalculada;
                    var pesos_ta = Array();
                    pesos_ta[1] = pesos_t[1] +( 0.6* Salidadiferencia * nejemplo[x][0]);
                    pesos_ta[2] = pesos_t[2] + (0.6* Salidadiferencia * nejemplo[x][1]);
                    pesos_ta[3] = pesos_t[3] + (0.6* Salidadiferencia * nejemplo[x][2]);
                    pesos_t[1] = pesos_ta[1];
                    pesos_t[2] = pesos_ta[2];
                    pesos_t[3] = pesos_ta[3];
                    var yini = -Math.round(pesos_t[1])-(Math.round(pesos_t[2])*-100) /Math.round(pesos_t[3]) ;
                    var yfin = -Math.round(pesos_t[1])-(Math.round(pesos_t[2])*400)/Math.round(pesos_t[3]) ;
                    var otrofinal = final + "Ejemplo actual (" + x + ") = {" + nejemplo[x][0] + "," + nejemplo[x][1] + "," + nejemplo[x][2] + "} <br>"+ "W = {"+pesos_t[1].toFixed(2) +","+pesos_t[2].toFixed(2) +","+ pesos_t[2].toFixed(2) +"}<br>";
                }
                imp(otrofinal)
            }
            function imp(otrofinal){
                document.getElementById("myid").innerHTML = otrofinal;
            }
            function iguales(pesos_epoca,pesos_t){
                for(var x in pesos_t){
                    if(pesos_t[x]!=pesos_epoca[x] ){
                        return false;
                    }
                }
                return true;
            }
            function dibPunto(canvas,x,y){
                var ctx = canvas.getContext("2d");
                ctx.fillStyle = "#ff0000";
                ctx.beginPath();
                var radio = 3;                                     // radio del arco
                ctx.arc(x,y,radio,0,2*Math.PI, true);
                ctx.fill();
            }
            function processFiles(files){
                var archivo = files[0];
                if ( archivo ) {
                    var reader = new FileReader();
                    reader.onload = function(e) {
                        var contents = e.target.result;
                        //document.getElementById("myid").innerHTML = "El archivo contiene:<br>" + contents; 
                        var canvas = document.getElementById("micanvas");
                        cuadricula(canvas);
                        
                        var entradas=0, ejemplos=0;
                        var b = new Array();
                        var Y = new Array();
                        var nejemplo = new Array( );
                        var res2 = contents.split(" ");//separados por espacios no retornos
                        var res3 = res2.toString();//
                        var res = res3.split("\n");//los separo por lineas
                        var c = res[0].split(",");
                        entradas = c[1];
                        ejemplos = c[0];
                        obtenerDatos(res, b, Y, nejemplo);
                        dibujarPuntos(canvas,nejemplo);
                        var epoca = 1;
                        var pesos_epoca = Array(0,0,0,0);
                        var pesos_t = Array(0,0,0,0);
                        while (epoca<=100){
                            var final = "Numero de ejemplos = " + ejemplos +"<br>";
                            final = final + "Numero de atributos = "+ entradas + "<br>";
                            final = final + "Epoca = "+ epoca + "<br>";
                            //var time = setInterval(function(){calcularPesos(final, nejemplo, pesos_t, Y,epoca);},2000);
                            calcularPesos( final, nejemplo, pesos_t, Y);
                            if(iguales(pesos_epoca,pesos_t)){
                                var yini = -Math.round(pesos_t[1])-(Math.round(pesos_t[2])*-100) /Math.round(pesos_t[3]) ;
                                var yfin = -Math.round(pesos_t[1])-(Math.round(pesos_t[2])*400)/Math.round(pesos_t[3]) ;
                                dibujalinea(canvas,yini,yfin);
                                break;
                            }
                            else{
                                pesos_epoca[1]=pesos_t[1];
                                pesos_epoca[2]=pesos_t[2];
                                pesos_epoca[3]=pesos_t[3];
                            }
                            epoca++;
                        }
                        var x  = nejemplo[ejemplos][1]*20;                    // coordenada x 
                        var y  = 400 - nejemplo[ejemplos][2]*20;              // coordenada y 
                        dibPunto(canvas,x,y);
                        //despDatos(arregloEpocas,ejemplos,entradas);
                    }
                    reader.readAsText(archivo);
                } else { 
                    alert("Error al abrir el archivo");
                }
            }
            document.getElementById('fileinput').addEventListener('change', readSingleFile, false);
            
        </script>
    </head>
    
    <body>
        
        <input type="file" id="fileinput" type="file" size="50" onchange="processFiles (this.files)"/>
        <section> 
        <div id="izquierda" style="text-align:center;">
            <center>
                <p id="myid" >
                </p><br>
                <p id="myid3" >
                </p><br>
            </center>
        </div>
        </section>
        
        <section> 
        <div id="izquierda" style="text-align:center;">
            <center>
                <canvas height="400px" width="400px" id="micanvas">
                </canvas>
                <p id="myid2">
                </p>
            </center>
        </div>
        </section>
    </body>
</html>

