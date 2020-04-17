<?php
    /* 
        Torres Bautista Fernando
        2013020038
        602-"A"
    */
    function signo($n){
        if($n>=0){
            return 1;
        }
        else{
            return -1;
        }
        
    }
    function clase($n){
        if($n>=0){
            return 1;
        }
        else{
            return 0;
        }
        
    }
    $archivo = fopen( "percep.txt", "r" );
    $n=0;
    $valores=array();
    while( !feof( $archivo )) {
        $linea = fgets( $archivo );
        $valores = explode(" ", $linea);
        if($n==0){
            $numero_ejemplos = $valores[0];
        }
        else{
            $i=0;
            $b[$n] = $valores[0];
            for($i=0; $i<count($valores); $i++){
                $ejemplo_n[$n][$i] = $valores[$i];
            }
            $Y[$n] = $valores[$i-1];
        }
        $n++;
    }
    
    /*echo "<center><big>Array</big><br><br>";
    print_r($numero_ejemplos);echo "<br>";
    print_r($ejemplo_n);echo "<br>";
    print_r($b);echo "<br>";
    print_r($Y);echo "<br></center>";
    
    $b =$_POST['b'];              //El arreglo $b, el arreglo $ejemplo_n y el arreglo $Y tienen un limite para el aprendizaje 
    $x1 = $_POST['x1'];             //ahora las variables x1, x2, x3, ..., nx estan en el arreglo ejemplo_n
    $x2 =$_POST['x2'];              //ahora las variables x1, x2, x3, ..., nx estan en el arreglo ejemplo_n
    $Y = $_POST['s'];               //Pasando el limite de aprendizaje estan los datos de prueba
                                    //El limite esta en la variable $numero_ejemplos, los siguientes son las pruebas
     */
    $pesos_epoca = array();
    for($i=1; $i<=$numero_ejemplos; $i++){
        $pesos_epoca[$i] = 0 ;
    }
    $epoca=1;
    $pesos_t=$pesos_epoca;
    //b * w1 + x1*w2 + ... + xn*wn
    while ( $epoca<=100 ){
        echo "<br><center>Epoca ".$epoca."<br>";
        $x_n=array();
        for($k=1; $k<=$numero_ejemplos; $k++){
            $x_n = $ejemplo_n[$k];
            $salidacalculada = 0;
            echo "Salidacalculada = ";
            for($j=1; $j<count($x_n); $j++){
                $salidacalculada = $salidacalculada + ( $pesos_t[$j]*$x_n[$j-1] );
                if( ($j+1) == (count($x_n))){
                    echo $pesos_t[$j]." * ".$x_n[$j-1]." = ".$salidacalculada."<br>";
                }
                else{
                    echo $pesos_t[$j]." * ".$x_n[$j-1]." + ";
                }
            }
            $Ycalculada = signo($salidacalculada);
            echo "Ycalculada = signo(".$salidacalculada.")=".$Ycalculada."<br>";
            $Salidadiferencia = $Y[$k] - $Ycalculada;
            echo "Salidadiferencia = ".$Y[$k] ."-". $Ycalculada." = ".$Salidadiferencia."<br>";
            for($i=1; $i<count($x_n); $i++){
                echo "w".$i." = ".$pesos_t[$i]."+ ( 0.6 * " . $Salidadiferencia ." * ". $x_n[$i-1]." )";
                $pesos_ta[$i] = $pesos_t[$i] + ( 0.6 * $Salidadiferencia * $x_n[$i-1] ) ;
                echo " = ".$pesos_ta[$i]."<br>";
            }
            $pesos_t = $pesos_ta;
            echo "W = ( ";
            for($i=1; $i<count($x_n); $i++){
                if($i==count($pesos_t)){
                    echo $pesos_t[$i]." )";
                }
                else{
                    echo $pesos_t[$i].", ";
                }
            }
            echo "<br><br>";
        }
        $epoca++;
        if($pesos_epoca == $pesos_t){
            echo "Termina en la epoca = ",$epoca."<br>";
            echo "Con los pesos de la siguiente forma.<br>W = ( ";
            for($i=1; $i<=count($pesos_t); $i++){
                if($i==count($pesos_t)){
                    echo $pesos_t[$i]." )<br><br>";
                }
                else{
                    echo $pesos_t[$i].", ";
                }
            }
            break;
        }
        else{
            $pesos_epoca = $pesos_t ;
        }//fin if/else
        echo "<br><br>";
    }//fin del while
    
    
    
    //Porcentaje de exactitud
    
    $correctos = 0;
    $incorrectos = 0;
    $contejemplo=0;
    $final=array();
    for(; $k<=count($ejemplo_n); $k++){
        $final = $ejemplo_n[$k];
        //Calcula la salida
        $salida=0;
        echo "Salida = ";
        for($i=1; $i<=count($pesos_epoca); $i++){
            $salida = $salida + $pesos_epoca[$i] * $final[$i-1];
            if( ($i+1) == (count($x_n))){
                echo $pesos_epoca[$i]." * ".$final[$i-1]." = ".$salida." Equivale a la clase =".clase($salida)." <br>";
            }
            else{
                echo $pesos_epoca[$i]." * ".$final[$i-1]." + ";
            }
        }
        if(clase($Y[$k]) == clase($salida) ){
            $correctos++;
        }
        else{
            $incorrectos++;
        }
        $contejemplo++;
    }
    $porcentajeExactitud = ($correctos/$contejemplo) * 100;
    echo "Así el porcecntaje de exactitud es de:<br>";
    
    echo "Porcentaje de Exactitud = ( ".$correctos." / ".$contejemplo." ) * 100 = ".$porcentajeExactitud;
    echo "<br><br></center>";
    
    