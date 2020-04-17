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
        <title> Modelo Vectorial </title>
        <style> 
        header { width: 100%; height: 40px; background: #FFFFFF; font-size: 2em; text-align: center; clear: both; }
        </style>
    </head>
    <body>
        <header >Modelo Vectorial</header>
        <FORM action="formV.php" method="post" enctype="multipart/form-data" style="text-align:center;" >    
            <!-- Funcion para obtener la frecuencia de terminos en cada doc -->
            <?php
                header('Content-Type: text/html; charset=UTF-8');  
                error_reporting (5);
                function cont_elementos( $i, $valores){
                    $n=0;
                    foreach($valores as $x){
                        if($x == $i){$n++;}
                    }
                    return $n;
                }
                function normalizar( $vecD ){
                    $arreglo = explode( " ", $vecD );
                    $mayor = max($arreglo);
                    foreach ($arreglo as $elemento){
                        $a_ret[/*$con*/] = number_format( $elemento/$mayor , 2 );//$numero;
                    }
                    array_shift($a_ret);//elimino el primer elemeno, por default es cero
                    return implode(" ",$a_ret);
                }
            ?>
            <!-- Primero obtenermos las tablas |t/d|d1|...|dn| y |d/t|t1|t2|...|tn| -->
            <?php
                $map=array();
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
            ?>
            <center>
            <?php
                echo "<table border=1 >";
                $n=1;
                foreach ($map as $x =>$x_valor){
                    if ($n % 2 == 0) {
                        echo "<tr bgcolor=#bdc3d6>";
                    } else {
                        echo "<tr>";
                    }
                    if ($n==1){
                        echo "<td>identificador</td><td>término</td><td>id del documento</td></tr>";
                    }
                    echo "<td>", "T",$n, "</td>";
                    //cambiar el x_valor a un array y formarlo con unique array para quitar los elementos repetidos de los id de doc
                    $arr = explode(" ",$x_valor);
                    $array2 = array_unique($arr);
                    $x_valor = implode(",",$array2);
                    echo "<td>", $x, "</td>","<td>", $x_valor, "</td>";
                    echo "</tr>";
                    $n=$n+1;
                }
                echo "</table><br>";
            ?>
            </center>
            <center>
            <?php
                for( $n1=1; $n1 <= 10; $n1++ ){
                    $vecD="";
                    foreach( $map as $x2_valor ){
                        $valores = explode(" ", $x2_valor );
                        ksort($valores);
                        if( in_array($n1, $valores) ){
                            $n = cont_elementos( $n1, $valores );
                            //echo "<td>", $n ,"</td>";
                            $vecD = $vecD." ".$n;
                        }
                        else{
                            //echo "<td>", "0" ,"</td>";
                            $vecD = $vecD." 0";
                        }
                    }
                    $vecN = normalizar( $vecD );
                    $map_bits[$n1] = $vecN;
                }
                echo "<table border=1 >";
                for( $n2=1; $n2 <= 10; $n2++ ){
                    if ($n2 % 2 == 0) {
                        echo "<tr bgcolor=#bdc3d6>";
                    } else {
                        echo "<tr>";
                    }
                    if ($n2==1){
                        echo "<td>", "doc/términos", "</td>";
                        for($i=1; $i<=$longitud; $i++){
                            echo "<td>", "T".$i , "</td>";
                        }
                        echo "</tr>";
                    }
                    echo "<td>", "Doc", $n2, "</td>";
                    $n_arr = explode(" ", $map_bits[$n2] );
                    foreach($n_arr as $elemento ){
                        echo "<td>", $elemento ,"</td>";
                    }
                }
                echo "</table><br>";
            ?>
            </center>
            <hr>
            <P>
            <LABEL> Consulta </LABEL> <INPUT type="text" name="consulta"><BR><BR>
            <LABEL> Elija el método para calcular coeficiente de similitud: </LABEL><BR>
            <INPUT type="radio" name="metodo" value="producto_e" checked> Producto escalar <BR>
            <INPUT type="radio" name="metodo" value="coseno" > Coseno del angulo <BR>
            <INPUT type="radio" name="metodo" value="dice" > Coeficiente de Dice <BR>
            <INPUT type="radio" name="metodo" value="jaccard" > Coeficiente de Jaccard <BR><BR>
            <INPUT type="submit" value="Enviar"> 
            <INPUT type="reset"><BR><BR>
            <?php echo '<textarea name="arreglo" style="visibility:hidden;">'.json_encode($map).'</textarea>';?>
            <?php echo '<textarea name="arreglo_dos" style="visibility:hidden;">'.json_encode($map_bits).'</textarea>';?>
            </P>
            <hr>
        </FORM>
    </body>
</html>

