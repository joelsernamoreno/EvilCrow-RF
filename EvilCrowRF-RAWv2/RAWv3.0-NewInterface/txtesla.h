const char TxTesla[] PROGMEM = R"=====(
<html>
        <head>
                <title>Wi-Fi Config</title>
                <meta charset="utf-8">
                <link rel="stylesheet" href="style.css">
                <script src="lib.js"></script>
        </head>
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
        <li><a href='/downloadlog'>Download Logs</a></li>
        <li><a href='/cleanspiffs'>Clean SPIFFS</a></li>
      </ul>
    </li>
    <li><a href='/txtesla'>Tesla Charge</a></li>
    <li><a href='/jammer'>Simple Jammer</a></li>
    <li><a class='dropdown-arrow'>ECRF Config</a>
      <ul class='sub-menus'>
        <li><a href='/wificonfig'>WiFi Config</a></li>
        <li><a href='/update'>OTA Firmware</a></li>
      </ul>
    </li>
  </ul>
</nav>
<br>
                <br>
<FORM action="/settxtesla" method="post" id="config" target="iframe">
    <p>Frequency: </p>
    <label class="custom-select" for="styledSelect1">
    <select name="frequency" form="config" id="styledSelect1">
       <option value="433.92">433.92MHz</option> 
       <option value="315.00">315MHz</option>
    </select>
    </label>
    <hr>
    <INPUT type="radio" name="configmodule" value="0" hidden="1" checked="checked">
    <INPUT class="button-submit" type="submit" value="Transmit">
</form>
<br>
<hr>
<br>
<iframe style="visibility: hidden;" src="http://" )+local_IPstr+"/settxtesla" name="iframe"></iframe>
</body>
</html>
)=====";
