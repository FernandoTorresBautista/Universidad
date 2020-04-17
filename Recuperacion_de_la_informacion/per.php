<!DOCTYPE html>
<!--
To change this license header, choose License Headers in Project Properties.
To change this template file, choose Tools | Templates
and open the template in the editor.
-->
<html>
    <head>
        <meta charset="UTF-8">
        <title></title>
    </head>
    <body>
        <center>
        <form enctype="multipart/form-data" action="perceptron.php" method="POST" >
        <LABEL>En un archivo introdusca los términos correspondientes a las columnas |b|x1|x2|...|xn|s: </LABEL><br><br>
        <LABEL>En el mismo archivo, después de los términos para el aprendizaje supervisado, </LABEL><br><br>
        <LABEL>incluir en en el mismo formato las pruebas, |b|x1|x2|...|xn|s:</LABEL><br><br>
        <input type="file" name="file[]" multiple ><br><br>
	<input type="submit" name="enviar" value="Analizar" >
        </form>
        </center>
    </body>
</html>
