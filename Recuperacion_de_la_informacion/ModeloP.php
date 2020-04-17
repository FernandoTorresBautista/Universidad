<!DOCTYPE html>
<!--
    Alumno: Torres Bautista Fernando
    Matricula: 2013020038
    Semestre: 2016-B
    Grupo: 602-A
-->
<html>
    <head>
        <meta charset="UTF-8">
        <title>Modelo Probabilístico</title>
    </head>
    <body>
        <header><Center><big>Modelo Probabilístico</big></center></header>
        <form action="formP.php" method="post" enctype="multipart/form-data" style="text-align:left;" >
            <?php
            error_reporting (5);
            //funcion para el conteo de frecuencias
            function cont_elementos( $i, $valores){
                $n=0;
                foreach($valores as $x){
                    if($x == $i){$n++;}
                }
                return $n;
            }
            //obtengo los términos de los documentos
            $map[""]="";
            //$map_bits;
            for( $i=1; $i <= 10; $i++ ){
                $nombre ="d".$i.".txt";
                $fp = fopen($nombre, "r");
                $total = fgets($fp);
                $n=1;
                while( $total>=$n ){
                    $linea = fgets($fp);
                    if (! empty($map["$linea"]) ){
                        $map["$linea"] = $map["$linea"]." ".$i;
                    } else {
                        $map["$linea"] = $i;
                    }
                    $n++;
                }
                fclose($fp);
            }
            unset($map[""]);
            $longitud = count($map);
            $ksort = ksort ( $map );
            //obtendo los vectores binarios correspondiente a cada término con respecto a los documentos
            $n1=1;
            $frecuencias=array();
            $n=0;
            foreach ($map as $x2 =>$x2_valor){
                $valores = explode(" ",$x2_valor);
                ksort($valores);
                $n=0;
                $arregloBinario="";
                for($cont=1; $cont<=10; $cont++){
                    if (in_array($cont, $valores)) {
                        //hago la suma de la frecuencia de los terminos con todos los documentos
                        $n = $n + cont_elementos( $cont, $valores );
                        $arregloBinario = $arregloBinario." 1";
                    }else{
                        $arregloBinario = $arregloBinario." 0";
                    }
                }
                $map_bits[$n1]=$arregloBinario;
                $frecuencias[$n1]=$n;
                $n1=$n1+1;
            }
            //Obtener los pesos de los terminos
            $w_pesos=array();
            $n3=1;
            foreach ($map_bits as $x_val){
                $valores_1 = explode(" ",$x_val);
                ksort($valores_1);
                $uno=1;
                $ni = cont_elementos($uno, $valores_1);
                $w_pesos[$n3] = /*log10( 0.5/1-0.5 ) +*/ log10(( 1-($ni/10)) / ($ni/10) ) ;
                $n3=$n3+1;
            }
            //Desplegar la tabla binaria con las columnas pesos y frecuencias aumentada
            echo "<center><table border=1>";
            $n2=1;
            foreach ($map as $x2 =>$x2_valor){
                if ($n2 % 2 == 0) {
                    echo "<tr bgcolor=#bdc3d6>";
                } else {
                    echo "<tr>";
                }
                if ($n2==1){
                    echo "<td><center>doc/términos<center></td><td>d1</td><td>d2</td><td>d3</td><td>d4</td>";
                    echo "<td>d5</td><td>d6</td><td>d7</td><td>d8</td><td>d9</td><td>d10</td>";
                    echo "<td>Pesos( W )</td><td>Frecuencia</td><td>2da Frecuencia</td></tr>";
                }
                $valores = explode(" ",$x2_valor);
                ksort($valores);
                echo "<td><center>", "T",$n2,"  (".$x2.")</center></td>";
                for($cont=1; $cont<=10; $cont++){
                    if (in_array($cont, $valores)) {
                        echo "<td>", "1" ,"</td>";
                    }else{
                        echo "<td>", "0" ,"</td>";
                    }
                }
                echo "<td>", $w_pesos[$n2] ,"</td>";
                echo "<td>", $frecuencias[$n2] ,"</td>";
                $cero=0;
                echo "<td> <input type='text'  size='6' name='frec[]' value='$frec'/> </td>";
                $n2=$n2+1;
            }
            echo "</table><br></center>";
            
            ?>    
            <center>
            <P>
            <label>Para acceder a las consulltas favor de poner el id de los términos separados por comas.<br><br></label>
            <LABEL> Consulta </LABEL> <INPUT type="text" name="consulta"><BR><BR>
            <LABEL> Datos para la segunda iteración.(De no ser necesaria dejarla todo en blanco) </LABEL><BR><BR>
            <LABEL> Numero de documentos reelevantes </LABEL> <INPUT type="text" name="N"><BR><BR>
            <INPUT type="submit" value="Enviar"> 
            <INPUT type="reset"><BR><BR>
            <?php //echo '<textarea name="arreglo_uno" style="visibility:hidden;">'.json_encode($map).'</textarea>';?>
            <?php echo '<textarea name="arreglo_dos" style="visibility:hidden;">'.json_encode($map_bits).'</textarea>';?>
            <?php echo '<textarea name="arreglo_tres" style="visibility:hidden;">'.json_encode($frecuencias).'</textarea>';?>
            <?php echo '<textarea name="arreglo_cuatro" style="visibility:hidden;">'.json_encode($w_pesos).'</textarea>';?>
            </P>
            </center>
        </form>
    </body>
</html>


