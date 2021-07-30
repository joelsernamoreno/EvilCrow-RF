const char btnconf[] PROGMEM = R"=====(
<html>
<head>
<title>RAW RX Config</title>
<meta charset="utf-8">
<link rel="stylesheet" href="style.css" />
</head>
<body>
<div class="setting">
<p>Button</p><input class="console" type="text" value="1"
onchange="fetch('/set.BUT?BUT=' + this.value)"></input>
<select class="console" onchange="fetch('/set.BUT?BUT=' + this.value)"
oninput="this.previousElementSibling.value = this.value;">
<option>1</option>
<option>2</option>
<option>3</option>
</select>
<p class="break">Select Button</p>
</div><BR>
<div class="setting">
<select id="Format" class="console" onchange="fetch('/set.BUT?BUT=' + this.value)"
oninput="this.previousElementSibling.value = this.value;">
<option class="PAR" Value="set.frq:">Set Frequency</option>
<option class="PAR">Set Deviation</option>
<option class="PAR">Set Modulation</option>
<option class="PAR">Set Bandwidth</option>
<option class="PAR">Run Wifi-Beacon-Spam</option>
<option class="PAR">Run TX-Value</option>
<option class="PAR">Run RX-Value</option>
</select>
<select id="Parameter" class="console" onchange="fetch('/set.BUT?BUT=' + this.value)"
oninput="this.previousElementSibling.value = this.value;">
<option class="PAR">+=</option>
<option class="PAR">-=</option>
<option class="PAR">=</option>
</select>
<input type="text" value="812" class="console" id="BAW"></input>
<a href="#" onclick="add(document.getElementById('Format').text);">Add Step</a>
</div>
<p></p>
<p></p>
<div id="log" class="log">
<ol id="Actions">
<li><a href="#" onclick="remove(this);">Home</a></li>
</ol> 
</div>
<BR><BR>  
<script>
function remove (el){
el.parentNode.remove ();
}
function add (el){
var li = document.createElement("li");
var _a = document.createElement("a"); li.append(_a);
_a.innerHTML = "Hallo"; _a.setAttribute("href","#");
_a.setAttribute ("onclick","remove(this);");
document.getElementById("Actions").append(li);
}
</script>
</body>
</html>
)=====";
