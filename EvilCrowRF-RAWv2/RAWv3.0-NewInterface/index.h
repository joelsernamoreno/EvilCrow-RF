const char Index[] PROGMEM = R"=====(
<html>
<head>
    <title>Evil Crow RF</title>
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
<br>
<hr>
<br>
<iframe style="visibility: hidden;" src="http://" )+local_IPstr+"/" name="iframe"></iframe>
</body>
</html>
)=====";
