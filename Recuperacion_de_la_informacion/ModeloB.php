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
        <title>Modelo Booleano</title>
        <style> 
        header { width: 100%; height: 40px; background: #FFFFFF; font-size: 2em; text-align: center; clear: both; } 
        section { position: relative; } 
        #izquierda { position: relative; width: 50%; top: 0; left: 0; background: #FFFFFF; float: left;  } 
        #derecha { position: relative; width: 50%; top: 0; right: 0; background: white; float: right;  } 
        </style> 
    </head>
    
    <body>
        <header>Modelo Booleano<br></header>
            <?php
            //Para no tener problemas con acentoss
            header('Content-Type: text/html; charset=UTF-8');  
            error_reporting (5);
            $map[""]="";
            $map_bits;
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
        <section> 
        <div id="izquierda" style="text-align:center;">
        <center>
        <?php
            echo "<table border=1>";
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
                echo "<td> T",$n, "</td>";
                echo "<td>", $x, "</td> <td>", $x_valor, "</td>";
                echo "</tr>";
                $n=$n+1;
            }
            echo "</table><br>";
            ?>
            </center>
        </div> 
        <div id="derecha" style="text-align:center;">
            <center>
        <?php
            echo "<table border=1>";
            $n1=1;
            foreach ($map as $x2 =>$x2_valor){
                if ($n1 % 2 == 0) {
                    echo "<tr bgcolor=#bdc3d6>";
                } else {
                    echo "<tr>";
                }
                if ($n1==1){
                    echo "<td>doc/términos</td><td>d1</td><td>d2</td><td>d3</td><td>d4</td><td>d5</td>";
                    echo "<td>d6</td><td>d7</td><td>d8</td><td>d9</td><td>d10</td></tr>";
                }
                $valores = explode(" ",$x2_valor);
                ksort($valores);
                echo "<td> T",$n1," </td>";
                $arregloBinario="";
                for($cont=1; $cont<=10; $cont++){
                    if (in_array($cont, $valores)) {
                        echo "<td> 1 </td>";
                        $arregloBinario = $arregloBinario."1";
                    }else{
                        echo "<td> 0 </td>";
                        $arregloBinario = $arregloBinario."0";
                    }
                }
                $map_bits[$n1]=$arregloBinario;
                $n1=$n1+1;
            }
            echo "</table><br>";
        ?>
            </center>
        </div> 
            <form action="#" method="post" enctype="multipart/form-data" style="text-align:center;"><br>
                <label>Ingrese su consulta:</label>
                <input type="text" name="consulta" > <br><br>
                <input type="submit" name=ok value="Enviar"> 
            </form>
        </section>
        <?php
            function isOp( $op ){
                if($op=="|"||$op=="&"||$op=="("||$op==")")
                    return true;
                return false;
            }
            
            function precedencia( $ele, $top ){
                $pr1=0;
                $pr2=0;
                if($ele=="&"){ $pr1 = $pr1 + 1; }
                if($ele=="|"){ $pr1 = $pr1 + 2; }
                if($top=="&"){ $pr2 = $pr2 + 1; }
                if($top=="|"){ $pr2 = $pr2 + 2; }
                return $pr1-$pr2;
            }
            
            function valor_n($cade){
                global $map_bits;
                return $map_bits["$cade"];
            }
            
            function f_and($cad1, $cad2){
                $c_b1 = valor_n($cad1);
                $c_b2 = valor_n($cad2);
                $c3 = "";
                for($inicio=0; $inicio<strlen($c_b1); $inicio++){
                    $c3 = $c3.($c_b1[$inicio]*$c_b2[$inicio]);
                }
                return $c3;
            }
            
            function f_or($cad1, $cad2){
                $c_b1 = valor_n($cad1);
                $c_b2 = valor_n($cad2);
                $c3="";
                for($inicio=0; $inicio<strlen($c_b1); $inicio++){
                    if($c_b1[$inicio]=="0"&&$c_b2[$inicio]=="0"){
                        $c3 = $c3."0";
                    }else{
                        $c3 = $c3."1";
                    }
                }
                return $c3;
            }
            
            function evaluar($val){
                $v_pila = [];
                $token = strtok($val," \n\t");
                $arregl =[];
                while ($token !== false) {
                    $arregl[]=$token;
                    $token = strtok(" \n\t");
                }
                foreach($arregl as $ev_val ){
                    if( $ev_val!="&" && $ev_val!="|" ){
                        array_push($v_pila, $ev_val);
                    }else{
                        $b = array_pop($v_pila);
                        $a = array_pop($v_pila);
                        if( $ev_val=="&" ){
                            $resul = f_and($a, $b);
                        } else{
                            if( $ev_val=="|" ){
                                $resul = f_or($a, $b);
                            }
                        }
                        array_push($v_pila, $resul);
                    }
                }
                $resultadof = array_pop($v_pila);
                return $resultadof;
            }
            function infija_post( $arr ){
                //inicializar pila
                $pila = [];
                $salida="";
                foreach($arr as $elemento){
                    if(!isOp($elemento)){
                        $salida = $salida." ".$elemento;
                    }
                    else if( $elemento == "(" ){
                        array_push($pila, $elemento);
                    }
                    else if( $elemento==")" ){
                        $emp = count($pila);
                        $subtr = array_pop($pila);
                        while( $subtr!="(" && $emp>0 ){
                            $salida = $salida." ".$subtr;
                            $emp = count($pila);
                            $subtr = array_pop($pila);
                        }
                        if( $emp==0 && $subtr!="(" ){
                            array_push($pila, $subtr);
                        }
                    }else{
                        $emp = count($pila);
                        $subtr = array_pop($pila);
                        while( precedencia($elemento,$subtr)<0 && $subtr!="(" && $emp>0 ){
                            $salida = $salida." ".$subtr;
                            $subtr= array_pop($pila);
                        }
                        array_push($pila, $elemento);
                    }
                }
                while( !empty($pila) ){
                    $subtr = array_pop($pila);
                    $salida = $salida." ".$subtr;
                }
                return $salida; 
            }
            function resultado(){
                $consulta = isset($_POST["consulta"]) ? $_POST["consulta"]:null;
                $consulta = str_replace(" ","",$consulta );
                $consulta = str_replace("(", " ( ",$consulta );
                $consulta = str_replace(")", " ) ",$consulta );
                $consulta = str_replace("  ", " ",$consulta );
                $consulta = str_replace("&", " & " ,$consulta );
                $consulta = str_replace("|", " | ",$consulta );
                $consulta = str_replace("  "," ",$consulta );
                $consulta = str_replace("t", " ",$consulta );
                $consulta = str_replace("T", " ",$consulta );
                $consulta = str_replace("  "," ",$consulta );
                $tok = strtok($consulta, " \n\t");
                $arr=NULL;
                while ($tok !== false) {
                    $arr[]=$tok;
                    $tok = strtok(" \n\t");
                }
                if ($arr!=NULL){
                    $val =  infija_post($arr)  ;
                    //print "\t<br>Notacion postfija: ".$val."<br>";
                }
                $valorfinal = evaluar($val);
                if($valorfinal!=NULL){
                    echo "<br><center> Rersultado final de la evaluacion de la expresion: <big>".$valorfinal."</big> </center><br>";
                }
            }
            ?>
        <?php
            echo "<br><center>";
            $res=resultado();
            echo "</center><br>";
        ?>
    </body>
</html>
