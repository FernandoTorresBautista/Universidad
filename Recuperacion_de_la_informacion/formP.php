<html>
    <head>
        <meta charset="UTF-8">
        <title>Modelo Probabilístico</title>
    </head>
    <body>
<header><Center><big>Modelo Probabilístico</big></center></header>
    <?php
    error_reporting (5);
    function cont_elementos( $i, $valores){
        $n=0;
        foreach($valores as $x){
            if($x == $i){$n++;}
        }
        return $n;
    }
    header('Content-Type: text/html; charset=UTF-8');
    $mapa_b = json_decode($_POST["arreglo_dos"]); //el mapa_b contiene los vectores de documentos    
    $mapa_frecuencias = json_decode($_POST["arreglo_tres"]);
    $mapa_pesos = json_decode($_POST["arreglo_cuatro"]);
    
    //Frecuencias para los terminos reelevantes en la segunda iteracion    
    $frec_seg = $_POST["frec"];
    $N =(int) $_POST["N"];
    $consulta = $_POST['consulta']; 
    //para calcular la similiztus entre los
    $consulta = str_replace(" ","",$consulta );
    $consulta = str_replace("  ","",$consulta );
    $consulta = str_replace("t","",$consulta );
    $val_con=explode(",", $consulta);
    array_unique($val_con);
    $n=1;
    foreach($mapa_b as $key => $x ){
        if(in_array($key,$val_con)){
            $arr_bit_q[$n] = $x;
            $n++;
        }
    }
    $n1=1;
    foreach($mapa_pesos as $key1 => $x1 ){
        if(in_array($key1,$val_con)){
            $arr_w_q[$n1] = $x1;
            $n1++;
        }
    }    
    $n2=1;
    foreach($mapa_frecuencias as $x2 ){
        $arr_f2[$n2] = $x2;
        $n2++;
    }
    //Aqui actualizo los pesos para el segundo calculo de pesos
    $n5=1;
    foreach($frec_seg as $key5 => $x5 ){
        if(in_array($n5,$val_con)){
            $arr_f2[$n5] = $x5;
            $n5++;
        }
    }
    //calcular segundos pesos
    $n3=1;
    foreach ($mapa_b as $x_val ){
        $valores_1 = explode(" ",$x_val);
        ksort($valores_1);
        $uno=1;
        $ni = cont_elementos( $uno, $valores_1 );//doc reelevantes
        $fi = $arr_f2[$n3];
        $w_pesos2[$n3] = log10( (($ni/$N)+0.5)/(1-(($ni/$N)+0.5)) ) + log10(( 1-( ($fi-$ni+0.5)/(10-$N+1)) ) / (($fi-$ni+0.5)/(10-$N+1)) ) ;
        $n3++;
    }
    
    $n6=1;
    foreach($mapa_pesos as $key6 => $x6 ){
        if(in_array($key6,$val_con)){
            $arr_w_q2[$n6] = $x6;
            $n6++;
        }
    }
    
    //Para el calculo de la similitud necesito los arreglos de bits de los terminos
    //tambien el arreglo de pesos arreglo_cuatro
    echo "<big>Similitud entre la colección de los documentos y la consulta:<br></big>";
    for($i=1; $i<=10; $i++){
        echo "<br>simProb(d".$i.",q) = ";
        //primera iteracion
        $total=0;
        $total2=0;
        $v_m=1;
        foreach($arr_bit_q as $x_valor){
            $arreglo = explode(" ", $x_valor);
            if($v_m==count($arr_bit_q)){
                $total = $total + ( $arreglo[$i] * $arr_w_q[$v_m] );
                echo "".$arreglo[$i]." * ".$arr_w_q[$v_m]." = ".$total;
                //segunda iteracion
                $total2 = $total2 + ( $arreglo[$i] * $arr_w_q2[$v_m] );
            }
            else{
                $total = $total + ( $arreglo[$i] * $arr_w_q[$v_m] );
                $total2 = $total2 + ( $arreglo[$i] * $arr_w_q2[$v_m] );
                echo $arreglo[$i]." * ".$arr_w_q[$v_m]." + ";
                
            }
            $v_m++;
        }
        $str = "SimDoc(".$i.",q)";
        $mDoc[$str] = $total;
        $mDoc2[$str] = $total2;
    }
    natcasesort($mDoc);
    $final = array_reverse($mDoc);
    natcasesort($mDoc2);
    $final2 = array_reverse($mDoc2);
    echo "<br><br>";
    echo "Documentos de mayor a menor relevancia:<br>";
    foreach($final as $key3 => $val2){
        echo $key3."=".$val2."<br>";
    }
    echo "<br><br>Segunda iteración<br>Documentos de mayor a menor relevancia:<br>";
    foreach($final2 as $key4 => $val3){
        echo $key4."=".$val3."<br>";
    }
    ?>
        
    </body>
</html>


        
    
    