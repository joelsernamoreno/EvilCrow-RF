const char TXBinary[] PROGMEM = R"=====(
<html>
<head>
    <title>Binary TX Config</title>
    <meta charset="utf-8">
    <link rel="stylesheet" href="style.css">
    <script src="lib.js"></script> 
</head>
<body>
<body>
<nav id='menu'>
  <input type='checkbox' id='responsive-menu' onclick='updatemenu()'><label></label>
  <ul>
    <li><a href='/'>Home</a></li>
    <li><a class='dropdown-arrow'>Config</a>
      <ul class='sub-menus'>
        <li><a href='/txconfig'>RAW TX Config</a></li>
        <li><a href='/txbinary'>Binary TX Config</a></li>
        <li><a href='/rxconfig'>RX Config</a></li>
        <li><a href='/btnconfig'>Button TX Config</a></li>
      </ul>
    </li>
    <li><a class='dropdown-arrow'>RX Log</a>
      <ul class='sub-menus'>
        <li><a href='/viewlog'>RX Logs</a></li>
        <li><a href='/delete'>Delete Logs</a></li>
      </ul>
    </li>
  </ul>
</nav>
<br>
<FORM action="/settxbinary" method="post" id="config" target="iframe">
    <p>Frequency: </p><textarea class="css-input-text" form="config" rows="1" cols="50" name="frequency"></textarea>
    <hr>
    <p>Modulation: </p><textarea class="css-input-text" form="config" rows="1" cols="50" name="mod"></textarea>
    <hr>
    <p>Binary Data: </p><textarea class="css-input-text" form="config" rows="1" cols="50" name="binarydata"></textarea>
    <hr>
    <p>Sample Pulse: </p><textarea class="css-input-text" form="config" rows="1" cols="50" name="samplepulse"></textarea>
    <hr>
    <p>Deviation: </p><textarea class="css-input-text" form="config" rows="1" cols="50" name="deviation"></textarea>
    <hr>
    <p>Transmissions: </p><textarea class="css-input-text" form="config" rows="1" cols="50" name="transmissions"></textarea>
    <hr>
    <INPUT type="radio" name="configmodule" value="6" hidden="1" checked="checked">
    <INPUT class="button-submit" type="submit" value="Transmit">
</form>
<br>
<hr>
<br>
<iframe style="visibility: hidden;" src="http://" )+local_IPstr+"/settxbinary" name="iframe"></iframe>
</body>
</html>
)=====";
