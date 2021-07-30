const char WebMain[] PROGMEM = R"=====(
<html>

<head>
<title>RAW RX Config</title>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link rel="stylesheet" href="style.css" />
</head>

<body>
<div id="container">
<nav>
<ul>
<li><a href="#">Sub-GHz Raw</a>
<ul>
<li><a href="#" onClick="NavigateFrame('/rxconfig');Hide(this)">RAW Receive</a></li>
<li><a href="#" onClick="NavigateFrame('/txconfig');Hide(this)">RAW Transmit</a></li>
</ul>
</li>
<li><a href="#">Sub GHz Attacks</a>
<ul>
<li><a href="#" onClick="NavigateFrame('/jammer');Hide(this)">Jammer</a></li>
<li><a href="#" onClick="NavigateFrame('/frqscan')">Frequency Scan</a></li>
<!--li><a href="#" onClick="NavigateFrame('');Hide(this)">KeyFob</a></li-->
<!--li><a href="#" onClick="NavigateFrame('');Hide(this)">OpenGarage</a></li-->
</ul>
</li>
<li>
<a href="#">Wifi Sniffers</a>
<ul>
<li>
<a href="#" onClick="NavigateFrame('/ScanWifi');Hide(this)">Show Wifi Networks</a></li>
<li>
<!--li>  <a href="#" onClick="NavigateFrame('');Hide(this)">Probe Sniffer</a></li>
<li>     <a href="#" onClick="NavigateFrame('');Hide(this)">Beacon Sniffer</a></li>
<li>     <a href="#" onClick="NavigateFrame('');Hide(this)">Deauth Sniffer</a></li>
<li>     <a href="#" onClick="NavigateFrame('');Hide(this)">EAPOL Logging</a></li>
<li>     <a href="#" onClick="NavigateFrame('');Hide(this)">PWNAGOTCHI Logging</a></li>
<li>     <a href="#" onClick="NavigateFrame('/ScanWifi');Hide(this)">Show Accesspoints</a>
</li-->
</ul>
</li>
<li>
<a href="#">System</a>
<ul>
<li><a href="#" onClick="NavigateFrame('/btncfg');Hide(this)">Button Config (Preview)</a></li>
<li><a href="#" onClick="NavigateFrame('/heap');Hide(this)">Heap-Status</a></li>
<li><a href="#" onClick="NavigateFrame('/reboot');Hide(this)">Reboot</a></li>
</ul>
</li>
<li>
<a href="#" onClick="NavigateFrame('');Hide(this)">Wifi Attacks</a>
<ul>
<li><a href="#" onClick="NavigateFrame('/RUN.WBF');Hide(this);alert('Device restarted for this Attack. Reset to normal by pressing Button 1');">Beacon Flooding (Offline)</a></li>
<li><a href="#" onClick="NavigateFrame('/RUN.IPC');Hide(this);alert('Device restarted for this Attack. Reset to normal by pressing Button 1');">Iphone-CrashAP  (Offline)</a></li>
<!--li> <a href="#" onClick="NavigateFrame('');Hide(this)">Deauth-Flooding</a></li>
<li>    <a href="#" onClick="NavigateFrame('');Hide(this)">Probe-Flooding</a></li>
<li>    <a href="#" onClick="NavigateFrame('');Hide(this)">Captive Portal</a></li>
<li>    <a href="#" onClick="NavigateFrame('');Hide(this)">EVIL-Twin</a></li-->
</ul>
</li>
<li>
<a href="#" onClick="NavigateFrame('');Hide(this)">Bluetooth Attacks</a>
<ul>
<li><a href="#" onClick="NavigateFrame('/ScanBT');Hide(this)">Scan Devices</a></li>
<!--li><a href="#" onClick="NavigateFrame('');Hide(this)">Scan Skimmers</a></li-->
<!--li><a href="#" onClick="NavigateFrame('');Hide(this)">Keyboard Emulator</a></li-->
</ul>
</li>
<!--li>
<a href="#" onClick="NavigateFrame('');Hide(this)">USB Attacks</a>
<ul>
<li>
<a href="#" onClick="NavigateFrame('');Hide(this)">RubberDucky by Emu USB</a></li>
</ul>
</li-->
</ul>
</nav>
</div>

<Iframe ID="CFrame" Height="60%" src="/rxconfig"></Iframe>
<BR>
<HR>
<a class="Btn" href="#" onclick="document.getElementById ('log').innerHTML = '';localStorage.removeItem('log')">Delete Log</a>
<a class="Btn" href="#" onclick="Download(document.getElementById('log').innerHTML)">Save Log</a>
<BR>
<Div class="log" id="log"></div>

<script type="Text/Javascript">
function NavigateFrame(URL){document.getElementById("CFrame").src=URL;};
function Hide (el){el.parentNode.parentNode.style.display = "none";setTimeout(function(){el.parentNode.parentNode.style.display = "";}, 500);};
function Download(text) {
var file = new Blob([text], {type: "text/plain"});
var a = document.createElement("a");
a.href = URL.createObjectURL(file);
a.download = "Log.txt";
document.body.appendChild(a);
a.click();
setTimeout(function() {
document.body.removeChild(a);
window.URL.revokeObjectURL(url);  
}, 0); 
};
</Script>
<script type="Text/Javascript">
function SetFRQ (val) {document.getElementById ('FRQ').value = val;     document.getElementById ('txtFRQ').value = val;}
function ExtractInfo (Text) {
var P1 = Text.split (":");
if (P1[0] == "SER.SET.FRQ.#1") {SetFRQ(P1[1])};
}
if (!!window.EventSource) {
var source = new EventSource('/events');

source.addEventListener('open', function(e) {
console.log("Events Connected, Loading last Logs from localStorage");
document.getElementById("log").innerHTML = localStorage.getItem ('log'); // Reset and Get
}, false);

source.addEventListener('error', function(e) {console.log("Events Error");}, false);

source.addEventListener('message', function(e) {
var P1 = e.data.split (":");
if (P1[0] != "FRQSCAN") {
document.getElementById("log").innerHTML = e.data + "<BR>" + document.getElementById("log").innerHTML;
} else {
document.getElementById("log").innerHTML = P1[0] + "<BR>" + document.getElementById("log").innerHTML;
};
localStorage.setItem ('log',document.getElementById("log").innerHTML);
}, false);

}
</script>
</body>

</html>
)=====";
