<?php
    //Para poder reconocer palabras acentuadas
    header('Content-Type: text/html; charset=ISO-8859-1');
    //header('Content-Type: text/html; charset=UTF-8');  
    $archivo = fopen( "PalabrasInvalidas.txt", "r" ) or exit( "Imporsible abrir el diccionario" );
    $i = 0;
    while( !feof( $archivo )) {
        $linea = fgets( $archivo );
        $palabrasinv[ $i++ ] = trim( $linea );
    }
    //Lectura de cada uno de los archivos a analizar
    $frecuecia = array();
    $j = 0;
    foreach ($_FILES["file"]["tmp_name"] as $filename) {
        $archivo = fopen( "$filename", "r" );
        $k = 0;
        $archivos = array();
        while( !feof( $archivo )) {
            $linea = fgets( $archivo );
            trim( $linea );
            $tokens = strtok( $linea, " ,.-:;_{}[]^*+¿?¡!|°/&%\#()-`~¨´\\'=$\"¬" );
            while( $tokens !== false ) {
                $tokMinuscula = strtolower( $tokens );
                if( !in_array( $tokMinuscula, $palabrasinv ) && strlen( $tokMinuscula ) > 2 ){
                    $documentos[ $j ][ $k++ ] = $tokens;
                    if( !array_key_exists( $tokens, $frecuecia )){
                        $frecuecia[ $tokens ] = 1;
                    }
                    else{
                        $frecuecia[ $tokens ] += 1;
                    }
                }
                $tokens = strtok( " ,.-:;_{}[]^*+¿?¡!|°/&%\#()-`~¨´\\'=$\"¬" );
            }
        }
        $j++;
    }
    
    //Para quitar palabras repetidas en un archivo
    $docs = array();
    for($i=0;$i<count($documentos);$i++) {
        $k = 0;
        for($j=0;$j<count($documentos[$i]);$j++) {
            if(empty($docs[$i]) || !in_array($documentos[$i][$j], $docs[$i])){
                $docs[$i][$k++] = $documentos[$i][$j];
            }
        }
    }
    //Ordenar con respecto a las palabras
    ksort($frecuecia);
    //Primero desplegar las tablas de cada documento las tablas solo contienen sustantivos
    echo "<br>Tablas de sustantivos que pasaron el filtro de cada documento<br><br>";
    for($j=0; $j<count($docs); $j++ ){
        echo "Documento".($j+1)."<br>";
        echo "<table border=1>";
        $n=1;
        foreach($docs[$j] as $x){
            if ($n % 2 == 0) {
                echo "<tr bgcolor=#bdc3d6>";
            } else {
                echo "<tr>";
            }
            if ($n==1){
                echo "<td>id</td><td>termino</td></tr>";
            }
            echo "<td> ".$n."</td>","<td> ".$x."</td>";
            $n++;
        }
        echo"<br>";
        echo "</table><br>";
    }
    //la frecuencia total de los documentos
    echo "Frecuencia total de terminos<br><br>";
    echo "<table border=1>";
    $n=1;
    foreach($frecuecia as $key=>$valor){
        if ($n % 2 == 0) {
            echo "<tr bgcolor=#bdc3d6>";
        } else {
            echo "<tr>";
        }
        if ($n==1){
            echo "<td>id</td><td>termino</td><td>frecuencia</td></tr>";
        }
        echo "<td> ".$n."</td>"."<td> ".$key."</td>"."<td> ".$valor."</td>";
        $n++;
    }
    echo "</table><br>";
    
    


