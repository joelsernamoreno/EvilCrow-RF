const char RXConfig[] PROGMEM = R"=====(
<html>
<head>
    <title>RAW RX Config</title>
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
<br>
<FORM action="/setrx" method="post" id="config" target="iframe">
    <p>Frequency: </p><textarea class="css-input-text" form="config" rows="1" cols="50" name="frequency"></textarea>
    <hr>
    <p>RxBW: </p><textarea class="css-input-text" form="config" rows="1" cols="50" name="setrxbw"></textarea>
    <hr>
    <p>Modulation: </p><textarea class="console" form="config" rows="1" cols="50" name="mod"></textarea>
    <hr>
    <p>Deviation: </p><textarea class="css-input-text" form="config" rows="1" cols="50" name="deviation"></textarea>
    <hr>
    <p>Data Rate: </p><textarea class="css-input-text" form="config" rows="1" cols="50" name="datarate"></textarea>
    <hr>
    <INPUT type="radio" name="configmodule" value="5" hidden="1" checked="checked">
    <INPUT class="button-submit" type="submit" value="Apply">
</form>
<br>
<hr>
<br>
<iframe style="visibility: hidden;" src="http://" )+local_IPstr+"/setrx" name="iframe"></iframe>
</body>
</html>
)=====";
