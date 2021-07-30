const char FRQScan[] PROGMEM = R"=====(
<html>
<head>
<title>RAW RX Config</title>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link rel="stylesheet" href="style.css" />
</head>
<body>

<div class="setting">
<p>Frequency</p>
<input class="console" type="text" id="txtFRQ" value="433.92" onchange="fetch('/set.FRQ?FRQ=' + this.value); frq = this.value;"></input>
<input type="range" min="300" max="928" value="433.92" class="slider console" id="FRQ"
oninput="this.previousElementSibling.value = this.value + '.00';"
onchange="fetch('/set.FRQ?FRQ=' + this.value); frq = this.value;">
</input>
<p class="break">Bands 300.0 to 348.0 MHz, 387.0 to 464.0 MHz and 779.0 to 928.0 MHz</p>
</div>
<div class="setting">
<p>Settle Time</p>
<input class="console" type="text" id="txtMSS" value="400" onchange="fetch('/set.MSS?MSS=' + this.value);"></input>
<input type="range" min="1" max="1000" value="400" class="slider console" id="MSS"
oninput="this.previousElementSibling.value = this.value;"
onchange="fetch('/set.MSS?MSS=' + this.value);">
</input>
<p class="break">Freq settle time in Microseconds</p>
</div>
<div class="setting">
<p>Resolution</p>
<input class="console" type="text" id="txtRES" value="0.05" onchange="fetch('/set.RES?RES=' + this.value);"></input>
<input type="range" min="1" max="100" value="5" class="slider console" id="RES"
oninput="this.previousElementSibling.value = parseInt(this.value) / 100;"
onchange="fetch('/set.RES?RES=' + parseInt(this.value) / 100);">
</input>
<p class="break">Frequency jump in MHz</p>
</div>
<BR>
        
<canvas width="400" height="200" id='c1'></canvas><BR><HR>
<canvas width="400" height="200" id='c2'></canvas><BR><HR>

<script type="text/javascript">
var ESP   = false;
var CMP   = "000111222333444555666777888999ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz{{||}}~~[[\\]]^^__``::;;<<==>>??@@!!##$$%%&&(())**++,,--..//";
var cnt   = 1;
var frq   = document.getElementById ('txtFRQ').value;
var c1    = document.getElementById ('c1');
var c2    = document.getElementById ('c2');
var ct1   = c1.getContext("2d");
var ct2   = c2.getContext("2d");

function DrawMatrix (canvas){
// ---------------------------
// Texte generieren
// ---------------------------
canvas.font = "12px Arial";
canvas.fillStyle = "#FFFFFF";
canvas.fillText ("Center",180,24);
canvas.fillText ((parseFloat(frq) - parseFloat(64 * 0.05)).toPrecision(3) ,5,12);
canvas.fillText (frq,180,12);
canvas.fillText ((parseFloat(frq) + parseFloat(64 * 0.05)).toPrecision(3) ,360,12);

canvas.font = "10px Arial";
canvas.fillText ("DIV:",5,140);
canvas.fillText ("MIN:",5,156);
canvas.fillText ("MAX:",5,166);
canvas.fillText ("AVG:",5,178);
canvas.fillText ("MED:",5,190);
canvas.fillText ("BR:",360, 190);

for (var i = 1; i <= 8; i++)  {drawLine(canvas,0,i*25,400,i*25, "lightgrey", 0.2);};
for (var i = 1; i <= 15; i++) {drawLine(canvas,i * 25,0,1 + (i * 25), 200, "lightgrey", 0.2);};
drawLine(canvas,200,0,200,400, "white", 0.4);
}

function ClearCanvas (canvas){
canvas.fillStyle = "#000000";
canvas.fillRect(0, 0, 400, 200); 
}

function drawLine (canvas, x1,y1,x2,y2, col, wid){
canvas.lineWidth = wid;
canvas.fillStyle = col;
canvas.strokeStyle = col;
canvas.beginPath();
canvas.moveTo(x1,y1);
canvas.lineTo(x2,y2);
canvas.stroke();
}

function setVal1 (canvas, params){
ClearCanvas (canvas);
DrawMatrix(canvas);
var min = 255;
var max = 0;
var avg = 0;
var med = [0];

if (params == "0"){return};
var P1 = params.split ("");
var dis = (400 - (P1.length * Math.round(400 / P1.length))) / 2;
for (var i = 0; i <= P1.length - 5; i++) {
canvas.fillStyle = "HSL(100, 100%, 50%)";
canvas.fillRect(dis + (i * 3), CMP.indexOf(P1[i]), 2, 200); // Punkte
if (P1[i] < min){min = CMP.indexOf(P1[i])};
if (P1[i] > max){max = CMP.indexOf(P1[i])};
avg += CMP.indexOf(P1[i]);
med.push (CMP.indexOf(P1[i]));
};  

canvas.font = "10px Arial";
canvas.fillStyle = "yellow";canvas.fillText (max,35,156);
canvas.fillStyle = "yellow";canvas.fillText (min,35,166);
canvas.fillStyle = "yellow";canvas.fillText (Math.round(avg / P1.length),35,178);
canvas.fillStyle = "yellow";canvas.fillText (med.slice().sort((a, b) => a - b)[Math.floor(med.length / 2)],35,190);
drawLine(canvas,0,min,400,min, "yellow", 0.7)
drawLine(canvas,0,max,400,max, "yellow", 0.7)
drawLine(canvas,0,avg,400,avg, "yellow", 0.7)
drawLine(canvas,0,med,400,med, "yellow", 0.7)
DrawMatrix(canvas);     
}

function setVal2 (canvas, params){
if (params == "0"){return};
var P1 = params.split ("");
var dis = (400 - (P1.length * Math.round(400 / P1.length))) / 2;
canvas.putImageData(canvas.getImageData (0,0, 400, 199), 0, 1);
canvas.fillStyle = "#000000";
canvas.fillRect(0,0, dis, 3);         // linken Rand schwarz auffüllen
canvas.fillRect(400 - dis,0, dis, 3);   // rechten Rand schwarz auffüllen
for (var i = 0; i < P1.length; i++) {
canvas.fillStyle = "HSL(" + (255 - (CMP.indexOf(P1[i]))) + ", 100%, 50%)";
canvas.fillRect(dis + (i * 3), 0, 3, 3);
};

cnt++;
if (cnt >= 100){cnt = 0;document.getElementById("log").innerHTML += "<BR>"; drawLine(canvas,0,3,400,3, "lightgrey", 0.3)};
for (var i = 1; i <= 15; i++) {drawLine(canvas,i * 25,2,i * 25,3, "lightgrey", 0.3);};
}

function InitEvents(){
if (!!window.EventSource) {
var source = new EventSource('/events');
source.addEventListener('open',     function(e) {console.log("Events Connected");}           , false);
source.addEventListener('FS',  function(e) {
setVal1(ct1,e.data);setVal2(ct2,e.data);
document.getElementById("log").innerHTML += ".";
}, false);
source.addEventListener('error',    function(e) {
console.log("Events Error occured");
if (e.target.readyState != EventSource.OPEN){InitEvents();}
}, false);
}
}

InitEvents();
ClearCanvas (ct1);  ClearCanvas (ct2);
setVal1(ct1,"0");   setVal2(ct2,"0");
</script>

</body>
</html>
)=====";
