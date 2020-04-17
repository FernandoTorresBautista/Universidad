<html>
  <head>
	<script type="text/javascript">
		var canvas;
		var context;
		var div;
		var angulo=0;
		window.onload=function(){
			canvas=document.getElementById("canvitas");
			context=canvas.getContext("2d");
			setInterval(dibuja,100);
		}
		function borrar(){
			context.clearRect(0,0,canvas.width,canvas.height);
		}
		function dibuja(){
			context.save();
			borrar();
			context.translate(canvas.width/2,canvas.height/2);
			context.rotate(angulo*Math.PI/180);
			context.fillStyle="green";
			context.fillRect(50,50,50,50);
			context.restore();
			angulo = angulo + 20;
		}
	</script>		
  </head>
  <body>
	<canvas id="canvitas" width="400" height="400">
	error
	</canvas>
  </body>
</html>
