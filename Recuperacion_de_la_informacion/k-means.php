<!DOCTYPE html>
<html>
    <head>
        <meta charset="UTF-8">
        <title>K-means</title>
        <style> 
            header { width: 100%; height: 40px; background: #FFFFFF; font-size: 2em; text-align: center; clear: both; } 
            section { position: relative; } 
            #izquierda { position: relative; width: 50%; top: 0; left: 0; background: #FFFFFF; float: left;  } 
            #derecha { position: relative; width: 50%; top: 0; right: 0; background: #FFFFFF; float: right;  } 
        </style> 
        <script type="text/javascript">
            //Los colores para distinguir los centroides
            var color = Array();//Array("#800080","#ffff00","#808000","#ff0000","#80808080","#aaaacc","#aaccaa","#ffffcc","#aaffdd","#eeeeff");//purpura, amarillo, oliva, rojo, gris
            var matriz_puntos = Array();
            var matriz_centroides = Array();
            var distacias_c = Array();
            var nc=0, np=0, n_ejemplo=0, p_p_c="", iteracion=0, myVar;
            
            function colores(){
                for (var i = 0; i < nc; i++) {
                    color[i] = '#' + Math.floor(Math.random()*16777215).toString(16);
                }
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
                    ctx.arc(x,y,5,0,2*Math.PI, true);
                    ctx.fillStyle = "#0000ff";
                    ctx.strokeStyle = "#0000ff";
                    ctx.fill();
                    ctx.stroke();
                }
            }
            function dibujarCentroides(ctx){
                for ( var x2 in matriz_centroides ){
                    ctx.beginPath();
                    var x        = matriz_centroides[x2][0]*12;               //pos x
                    var y        = 600 - matriz_centroides[x2][1]*12;         //pos y
                    
                    ctx.arc(x,y,6.5,0,2*Math.PI, true);
                    ctx.fillStyle = color[x2];
                    ctx.strokeStyle = color[x2];
                    ctx.fill();
                    ctx.stroke();
                }
            }
            function distancias(){
                var distancias = Array();
                var datos  = "Distancias: <br>";
                for ( var i=0; i<nc; i++ ){
                    datos = datos + "Centroide "+(i+1)+" <br>";
                    distancias[i] = Array(); //Distancias del centroide 1 con todos los puntos
                    for ( var j=0;j<np;j++ ){
                        var dis = Math.sqrt(Math.pow(Math.abs(matriz_centroides[i][0]-matriz_puntos[j][0]),2)
                            + Math.pow(Math.abs(matriz_centroides[i][1]-matriz_puntos[j][1]),2));
                        distancias[i][j]=dis;
                        datos = datos + "d( c"+ (i+1) +", p"+ (j+1) +") = "+dis+"<br>";
                    }
                }
                document.getElementById('ejemplo0').innerHTML = datos;
                return distancias;
            }
            function init(){
                var matriz_c=Array();
                for(var i=0; i<nc; i++){
                    matriz_c[i]=Array();
                }
                return matriz_c;
            }
            function a_centroide(x2){
                var menor = 10000000;//numero demasiado grandre
                var indice = 0;
                for (var x=0; x<nc; x++){
                    if( distancias_c[x][x2] < menor ){
                        menor = distancias_c[x][x2];
                        indice = x;
                    }
                }
                return indice;
            }
            function iguales(n_centroides){
                for (var i=0; i<nc;i++){
                    if(n_centroides[i][0]!=matriz_centroides[i][0] || n_centroides[i][1]!=matriz_centroides[i][1]){
                        return 0;
                    }
                }
                return 1;
            }
/*            
            function pro_cruz(o ,a, b){
                return (a[0] - o[0]) * (b[1] - o[1]) - (a[1] - o[1]) * (b[0] - o[0]);
            }
            function convex_h( puntos ){
                document.getElementById('ejemplo2').innerHTML = "<br>antes de ordenar p =<br> " + puntos ;
                puntos.sort(function(a, b) {
                    return (a[0] == b[0]) ? (a[1] - b[1]) : (a[0] - b[0]);
                });
                if(puntos.length<=3)
                    return puntos;
                document.getElementById('ejemplo1').innerHTML = "despues de ordenar P = <br>" + puntos;
                var p_bajos=[];
                for (var i = 0; i < puntos.length; i++) {
                    while ( (p_bajos.length >= 2) && ( pro_cruz(p_bajos[p_bajos.length - 2], p_bajos[p_bajos.length - 1], puntos[i]) <= 0) ) {
                        p_bajos.pop();
                    }
                    p_bajos.push(puntos[i]);
                }
                var p_altos=[];
                for (var i = puntos.length-1; i > 0; i--) {
                    while ( (p_altos.length >= 2) && (pro_cruz(p_altos[p_altos.length - 2], p_altos[p_altos.length - 1], puntos[i]) <= 0) ) {
                        p_altos.pop();
                    }
                    p_altos.push(puntos[i]);
                }
                //p_altos.pop();
                p_bajos.pop();
                var ret = [];
                for (var p in p_bajos)
                    ret.push(p_bajos[p]);
                for (var p in p_altos)
                    ret.push(p_altos[p]);
                document.getElementById('ejemplo0').innerHTML = "<br>p bajos:<br> " + p_bajos + "<br>p altos:<br>"+ p_altos;
                return ret;
            }
*/          
            function puntos_de( pinc ){
                var centroide = "";
                for(var i=0; i<nc;  i++){
                    centroide += "C"+(i+1)+" = ( ";
                    for(var j=0; j<np; j++){
                        if( pinc[j]==i ){
                            centroide += (j+1) + " ";
                        }
                    }
                    centroide += ")<br>";
                }
                document.getElementById('datos').innerHTML = centroide;
            }
            function iterapuntos(ctx){ 
                distancias_c = distancias();        //Se calculan las distancias de los centroides a todos los puntos
                var pinc=Array();
                for ( var x2=0; x2<np; x2++ ){
                    pinc[x2] = a_centroide(x2);   //Gana el centroide en la posicion retornada y por tanto es el color del punto
                    var x = matriz_puntos[x2][0]*12;
                    var y = 600 - matriz_puntos[x2][1]*12;
                    ctx.beginPath();
                    ctx.arc(x,y,4,0,2*Math.PI, true);
                    ctx.strokeStyle = color[pinc[x2]];
                    ctx.fillStyle = color[pinc[x2]];
                    ctx.stroke();
                    ctx.fill();
                }
                //Con esto calculo los nuevos centroides y los retorno en esta funcion
                var matriz_c = init();
                p_p_c="Centroides calculados: <br>";
                for(var i=0; i<nc; i++){
                    var sumax=0, sumay=0, divisor=0;
                    for(var j=0; j<np; j++){
                        if( pinc[j] == i ){
                            //p_p_c += " " + matriz_puntos[j][0];
                            sumax += parseInt(matriz_puntos[j][0]);
                            sumay += parseInt(matriz_puntos[j][1]);
                            divisor++;
                        }
                    }
                    matriz_c[ i ][0] = (sumax/divisor) ;
                    matriz_c[ i ][1] = (sumay/divisor) ;
                    p_p_c += " C"+(i+1)+" ( " + matriz_c[ i ][0] + " , " + matriz_c[ i ][1] + " )<br>";
                    document.getElementById('ejemplo').innerHTML = p_p_c;
                }
                var salida = "Iteracion "+(iteracion+1)+"<br>";
                document.getElementById('ejemplo').innerHTML = p_p_c;
                if( iguales( matriz_c ) ){
                    puntos_de( pinc );
                    document.getElementById('ejemplo').innerHTML = salida+"<br>Al final se tiene <br>" + p_p_c;
                    myStopFunction();
                    //Iniciar convex al final de todo
                    /*for( var j=0; j<nc; j++){
                        var puntos = [];
                        var cont=0;
                        for ( var x2=0; x2<np; x2++ ){
                            if( pinc[x2] == j ){
                                puntos[cont] = matriz_puntos[x2];
                                cont++;
                            }
                        }
                        var final_array = convex_h( puntos );
                        document.getElementById('ejemplo3').innerHTML = "Puntos finales : " + final_array;
                    }*/
                }
                else{
                    puntos_de( pinc );
                    document.getElementById('ejemplo').innerHTML = salida+"<br>"+p_p_c;
                    matriz_centroides = matriz_c;
                }
            }
            function animacion(ctx){
                ctx.clearRect(0,0,600,600);
                cuadricula(ctx);
                dibujarPuntos(ctx);
                dibujarCentroides(ctx);
                //Calcula la distancia de cada centroide a cada punto
                //decide cual es el mas cercano y lo pinta del color del centroide
                //Despues de pasar por todos los puntos se calculan nuevamente los centroides
                iterapuntos(ctx);
                //Hasta aquí tnego las agrupaciones de los centroides
                iteracion++;
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
                        
                        np = parseInt(matriz_puntos[0][0]);             //numero de puntos
                        nc = parseInt(matriz_puntos[0][1]);          //numero de centroides
                        
                        colores();
                        
                        matriz_centroides = matriz_puntos.slice( 1,nc + 1 ); //matriz de centroides
                        matriz_puntos = matriz_puntos.slice( nc+1, nc + np + 1 );
                        
                        var canvas = document.getElementById('canvas');
                        var ctx = canvas.getContext("2d");
                        
                        animacion(ctx);
                        myVar = setInterval(function(){animacion(ctx)}, 100);
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
                    <input type="file" id="files" size="50" onchange="processFiles(this.files)" ><br><br>
                    <p id="ejemplo" ></p><br><br>
                    <p id="datos" ></p>
                </center>
            </div>
        </section>
        <section>
            <div id="derecha">
                <center>
                    <canvas id="canvas" height="600px" width="600px" ></canvas><br>
                    <p id="ejemplo0" ></p>
                </center>
            </div>
        </section>
    </body>
</html>
