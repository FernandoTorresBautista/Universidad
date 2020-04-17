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
    <title>Análisis de archivos</title>
</head>
    <body>
    <center>Ingrese los archivos a ser análizados</center><br>
        <center>
        <form enctype="multipart/form-data" action="datos.php" method="POST" >
        <input type="file" name="file[]" multiple ><br><br>
	<input type="submit" name="enviar" value="Analizar" >
        </form>
        </center>
    </body>
</html>