const char RXConfig[] PROGMEM = R"=====(
<html>
<head>
<title>RAW RX Config</title>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link rel="stylesheet" href="style.css" />
</head>
<body>
<div id="header">
<div class="setting">
<p>Radio-Module</p><input class="console" type="text" value="1"  onchange="fetch('/set.RAD?RAD=' + this.value)"></input>
<select class="console"
onchange="fetch('/set.RAD?RAD=' + this.value)"
oninput="this.previousElementSibling.value = this.value;">
<option>1</option>
<option>2</option>
</select>
<p class="break">Select Radio Module</p>
</div>
<div class="setting">
<p>Frequency</p> <input class="console" type="text" id="txtFRQ" value="433.92"onchange="fetch('/set.FRQ?FRQ=' + this.value)"></input> 
<input type="range" min="300" max="928" value="433.92" class="slider console" id="FRQ"
oninput="this.previousElementSibling.value = this.value + '.00';"
onchange="fetch('/set.FRQ?FRQ=' + this.value)"> </input>
<p class="break">Bands 300.0 to 348.0 MHz, 387.0 to 464.0 MHz and 779.0 to 928.0 MHz</p>
</div>
<div class="setting">
<p>Bandwith</p> <input class="console" type="text" value="812" id="txtBAW"
onchange="fetch('/set.BAW?BAW=' + this.value)"></input> <input type="range" min="58" max="812"
value="812" class="slider console" id="BAW" oninput="this.previousElementSibling.value = this.value;"
onchange="fetch('/set.BAW?BAW=' + this.value)"></input>
<p class="break">Range from 58 to 812 kHz</p>
</div>
<div class="setting">
<p>Modulation</p><input class="console" type="text" value="0" id="txtMOD"
onchange="fetch('/set.MOD?MOD=' + this.value)"></input><select class="console" id="MOD"
onchange="fetch('/set.MOD?MOD=' + this.value)">
<option>[0] 2-FSK</option>
<option>[1] GFSK</option>
<option>[2] ASK/OOK</option>
<option>[3] 4-FSK</option>
<option>[4] MSK/QPSK</option>
</select>
<p class="break">ASK=Max:250,GFSK=Max:250,2FSK=Max:500,4-FSK=Max:300,MSK=Max:500</p>
</div>
<div class="setting">
<p>Deviation</p> <input class="console" type="text" value="47.6" id="txtDEV"
onchange="fetch('/set.DEV?DEV=' + this.value)"></input> <input type="range" min="1.58" max="380.85"
value="47.6" class="slider console" id="DEV" oninput="this.previousElementSibling.value = this.value;"
onchange="fetch('/set.DEV?DEV=' + this.value)"></input>
<p class="break">Range from 1.58 to 380.85 kHz</p>
</div>
<div class="setting">
<p>Bitrate</p> <input class="console" type="text" value="99.4" id="txtBPS"
onchange="fetch('/set.BPS?BPS=' + this.value)"></input> <input type="range" min="1" max="600"
value="99.4" class="slider console" id="BPS"
oninput="this.previousElementSibling.value = this.value + '.0';"
onchange="fetch('/set.BPS?BPS=' + this.value)">
<p class="break">Range from 0.025 to 600.0 kbps (Limited by modulation type)</p>
</div>
</div> <BR><BR> <a class="Btn" href="#" onclick="fetch('/RXRUN')">Start RX</a> <a class="Btn" href="#"
onclick="fetch('/RXEND')">Stop RX</a>
<script type="Text/Javascript">
function SetFMT(val){document.getElementById('FMT').value=val;document.getElementById('txtFMT').value=val;} function SetVAL(val){document.getElementById('VAL').value=val;document.getElementById('txtVAL').value=val;} function SetBAW(val){document.getElementById('BAW').value=val;document.getElementById('txtBAW').value=val;} function SetBPS(val){document.getElementById('BPS').value=val;document.getElementById('txtBPS').value=val;} function SetMOD(val){document.getElementById('MOD').value=val;document.getElementById('txtMOD').value=val;} function SetDEV(val){document.getElementById('DEV').value=val;document.getElementById('txtDEV').value=val;} function SetRAD(val){document.getElementById('RAD').value=val;document.getElementById('txtRAD').value=val;} function SetFRQ(val){document.getElementById('FRQ').value=val;document.getElementById('txtFRQ').value=val;} function ExtractInfo(Text){var P1=Text.split(":");if(P1[0]=="SER.SET.FMT.#1"){SetFMT(P1[1])};if(P1[0]=="SER.SET.VAL.#1"){SetVAL(P1[1])};if(P1[0]=="SER.SET.BAW.#1"){SetBAW(P1[1])};if(P1[0]=="SER.SET.BPS.#1"){SetBPS(P1[1])};if(P1[0]=="SER.SET.MOD.#1"){SetMOD(P1[1])};if(P1[0]=="SER.SET.DEV.#1"){SetDEV(P1[1])};if(P1[0]=="SER.SET.RAD.#1"){SetRAD(P1[1])};if(P1[0]=="SER.SET.FRQ.#1"){SetFRQ(P1[1])};if(P1[0]=="SER.SET.FMT.#2"){SetFMT(P1[1])};if(P1[0]=="SER.SET.VAL.#2"){SetVAL(P1[1])};if(P1[0]=="SER.SET.BAW.#2"){SetBAW(P1[1])};if(P1[0]=="SER.SET.BPS.#2"){SetBPS(P1[1])};if(P1[0]=="SER.SET.MOD.#2"){SetMOD(P1[1])};if(P1[0]=="SER.SET.DEV.#2"){SetDEV(P1[1])};if(P1[0]=="SER.SET.RAD.#2"){SetRAD(P1[1])};if(P1[0]=="SER.SET.FRQ.#2"){SetFRQ(P1[1])};} if(!!window.EventSource){var source=new EventSource('/events');source.addEventListener('open',function(e){console.log("Events Connected");},false);source.addEventListener('error',function(e){if(e.target.readyState!=EventSource.OPEN){console.log("Events Disconnected");}},false);source.addEventListener('message',function(e){ExtractInfo(e.data);console.log("message",e.data);},false);source.addEventListener('myevent',function(e){console.log("myevent",e.data);},false);}
</script>
</body>
</html>
)=====";
