const char TXConfig[] PROGMEM = R"=====(
<html>
<head>
    <title>RAW TX Config</title>
    <meta charset="utf-8">
    <style>
        body {
            background-color: #333333;
            font-family: "Century Gothic", Arial;
            color: white;
            margin: 20px;
        }
        .myButton:link, .myButton:visited {
            background: linear-gradient(#777777, #444444);
            color: white;
            padding: 4px;
            min-width: 100px;
            border-radius: 5px;
            border: 2px solid white;
            text-align: center;
            margin-right: 20px;
            text-decoration: none;
            display: inline-block;
            transition: 0.25s;
        }
        .myButton:hover, .myButton:active {
            background: linear-gradient(#888888, #555555);
            border: 2px solid deepskyblue;
            border-radius: 10px;
            transform:scale(1.15);
        }
        .submitBtn {
            background: linear-gradient(#777777, #444444);
            color: white;
            padding: 4px;
            min-width: 100px;
            border-radius: 2.5px;
            border: 2px solid white;
            text-align: center;
            transition: 0.25s;
        }
        .submitBtn:hover, .submitBtn:active {
            background: linear-gradient(#888888, #555555);
            border: 2px solid deepskyblue;
        }
        .console{
            width: 99%;
            background-color: black;
            color: white;
            font-family: monospace;
            padding: 5px;
            border: 0px;
        }
    </style>
</head>
<body>
<center>
<div id="header">
    <div id="menu">
        <ul>
            <a class="myButton" href="/">Menu</a>
            <a class="myButton" href="/rxconfig">RAW RX Config</a>
            <a class="myButton" href="/txconfig">RAW TX Config</a>
            <a class="myButton" href="/txbinary">Binary TX Config</a>
      <a class="myButton" href="/viewlog">RAW RX Log</a>
      <a class="myButton" href="/delete">Delete Log</a>
        </ul>
    </div>
</div>
<hr>
</center>
<br>
<FORM action="/settx" method="post" id="config" target="iframe">
    <p>Frequency: </p><textarea class="console" form="config" rows="1" cols="50" name="frequency"></textarea>
    <hr>
    <p>Modulation: </p><textarea class="console" form="config" rows="1" cols="50" name="mod"></textarea>
    <hr>
    <p>RAW Data: </p><textarea class="console" form="config" rows="1" cols="50" name="rawdata"></textarea>
    <hr>
    <p>Deviation: </p><textarea class="console" form="config" rows="1" cols="50" name="deviation"></textarea>
    <hr>
    <p>Transmissions: </p><textarea class="console" form="config" rows="1" cols="50" name="transmissions"></textarea>
    <hr>
    <INPUT type="radio" name="configmodule" value="5" hidden="1" checked="checked">
    <INPUT class="submitBtn" type="submit" value="Transmit">
</form>
<br>
<hr>
<br>
<iframe style="visibility: hidden;" src="http://" )+local_IPstr+"/setrx" name="iframe"></iframe>
</body>
</html>
)=====";
