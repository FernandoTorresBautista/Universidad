<?php
    error_reporting (5);
    header('Content-Type: text/html; charset=UTF-8');  
    function producto_escalar($vD, $vQ){
        //Producto escalar
        $total=0;
        for($i=0; $i<count($vD); $i++){
            $total = $total + $vD[$i]*$vQ[$i];
            if( count($vD)-1!=$i ){
                echo $vD[$i]." * ".$vQ[$i]." + ";
            }
            else{
                echo $vD[$i]." * ".$vQ[$i]." = ".$total."<br>";
                return $total;
            }
        }
        
    }
    
    function coseno($vD, $vQ){
        //coseno
        $res=0;
        $sum_pp=0;
        $sumXX=0;
        $sumYY=0;
        for($i=0; $i<count($vD); $i++){
            $sum_pp = $sum_pp + $vD[$i]*$vQ[$i];
            $sumXX = $sumXX + $vD[$i]*$vD[$i];
            $sumYY = $sumYY + $vQ[$i]*$vQ[$i];
        }
        $res = $sum_pp / sqrt( $sumXX * $sumYY );
        echo $sum_pp." / sqrt( ".$sumXX." * ".$sumYY." ) = ".$res;
        return $res;
        
    }
    
    function Dice($vD, $vQ){
        //Dice
        $res=0;
        $sum_pp=0;
        $sumXX=0;
        $sumYY=0;
        for($i=0; $i<count($vD); $i++){
            $sum_pp = $sum_pp + $vD[$i]*$vQ[$i];
            $sumXX = $sumXX + $vD[$i]*$vD[$i];
            $sumYY = $sumYY + $vQ[$i]*$vQ[$i];
        }
        $res = ( 2 * $sum_pp ) / sqrt( $sumXX * $sumYY );
        echo " 2 * ".$sum_pp." / sqrt( ".$sumXX." * ".$sumYY." ) = ".$res;
        return $res;
        
    }
    
    function Jaccard($vD, $vQ){
        //Jaccard
        $res=0;
        $sum_pp=0;
        $sumXX=0;
        $sumYY=0;
        for($i=0; $i<count($vD); $i++){
            $sum_pp = $sum_pp + $vD[$i]*$vQ[$i];
            $sumXX = $sumXX + $vD[$i]*$vD[$i];
            $sumYY = $sumYY + $vQ[$i]*$vQ[$i];
        }
        $res = $sum_pp / $sumXX + $sumYY - $sum_pp ;
        echo $sum_pp." / ".$sumXX." + ".$sumYY." - ".$sum_pp." = ".$res;
        return $res;
    }
    
    $mapa = json_decode($_POST["arreglo"]); //el mapa solo guarda las palabras como key y los id´s de los documentos donde se encuentran como valores
    $mapa_b = json_decode($_POST["arreglo_dos"]); //el mapa_b contiene los vectores de documentos
    $consulta = $_POST['consulta']; 
    $metodo = $_POST['metodo'];
    $arreglo = explode( ",", $consulta );
    $nuevo = array();
    //Quito los espacios al pricipio y al final de los string
    foreach($mapa as $key1 => $val0){
        //echo $key1." ".$val0."<br>";
        $nuevo[trim((string)$key1)] = trim((string)$val0);
    }
    //construir el vector de consulta
    $vecC="";
    foreach($nuevo as $key3 => $val2){
        if(in_array($key3, $arreglo)){
            $vecC = $vecC." 1";
        }
        else{
            $vecC = $vecC." 0";
        }
    }
    //elimino el primer elemento para tener el array a la par de los vectoresdoc con el de consulta.
    $vecQ = explode(" ", $vecC);
    array_shift($vecQ);
    switch( $metodo ){
        case 'producto_e':{
            echo "<Big>Similitud por Producto escalar</Big><br><br>";
            $mDoc=array();
            $cont=1;
            foreach($mapa_b as $key2 => $val1){
                $vecDoc = explode(" ", $val1);
                echo "Sim(Doc".$key2.",q) = ";
                $strr = "SimDoc(".$key2.",q)";
                $mDoc[$strr]=producto_escalar( $vecDoc, $vecQ );
                echo "<br><br>";
                $cont++;
            }
            //krsort($mDoc);
            natcasesort($mDoc);
            $final = array_reverse($mDoc);
            echo "Similitudes por orden ascendente:<br>";
            foreach($final as $key3 => $val2){
                echo $key3."=".$val2."<br>";
            }
            break;
        }
        case 'coseno':{
            echo "<Big>Similitud por Coseno del ángulo:</Big><br><br>";
            $mDoc=array();
            $cont=1;
            foreach($mapa_b as $key2 => $val1){
                $vecDoc = explode(" ", $val1);
                echo "Sim(Doc".$key2.",q) = ";
                $strr = "SimDoc(".$key2.",q)";
                $mDoc[$strr]=  coseno( $vecDoc, $vecQ );
                echo "<br><br>";
                $cont++;
            }
            natcasesort($mDoc);
            $final = array_reverse($mDoc);
            echo "Similitudes por orden ascendente:<br>";
            foreach($final as $key3 => $val2){
                echo $key3."=".$val2."<br>";
            }
            break;
        }
        case 'dice':{
            echo "<Big>Similitud por la fórmula de Dice</Big><br><br>";
            $mDoc=array();
            $cont=1;
            foreach($mapa_b as $key2 => $val1){
                $vecDoc = explode(" ", $val1);
                echo "Sim(Doc".$key2.",q) = ";
                $strr = "SimDoc(".$key2.",q)";
                $mDoc[$strr]= Dice( $vecDoc, $vecQ );
                echo "<br><br>";
                $cont++;
            }
            natcasesort($mDoc);
            $final = array_reverse($mDoc);
            echo "Similitudes por orden ascendente:<br>";
            foreach($final as $key3 => $val2){
                echo $key3."=".$val2."<br>";
            }
            break;
        }
        case 'jaccard':{
            echo "<Big>Similitud por la formula de Jaccard</Big><br><br>";
            $mDoc=array();
            $cont=1;
            foreach($mapa_b as $key2 => $val1){
                $vecDoc = explode(" ", $val1);
                echo "Sim(Doc".$key2.",q) = ";
                $strr = "SimDoc(".$key2.",q)";
                $mDoc[$strr]= Jaccard( $vecDoc, $vecQ );
                echo "<br><br>";
                $cont++;
            }
            natcasesort($mDoc);
            $final = array_reverse($mDoc);
            echo "Similitudes por orden ascendente:<br>";
            foreach($final as $key3 => $val2){
                echo $key3."=".$val2."<br>";
            }
            break;
        }
    }


    