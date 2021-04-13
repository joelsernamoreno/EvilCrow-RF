const char BruteForce[] PROGMEM = R"=====(
<html>
<head>
    <title>RX Config</title>
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
            <a class="myButton" href="/rxconfig">RX Config</a>
            <a class="myButton" href="/txconfig">TX Config</a>
            <a class="myButton" href="/bruteforce">Brute Force</a>
      <a class="myButton" href="/viewlog">RX Log</a>
      <a class="myButton" href="/delete">Delete Log</a>
        </ul>
    </div>
</div>
<hr>
</center>
<br>
<FORM action="/setbruteforce" method="post" id="brute" target="iframe">
    <p>Frequency: </p><textarea class="console" form="brute" rows="1" cols="50" name="frequency"></textarea>
    <hr>
    <p>Code: </p><textarea class="console" form="brute" rows="1" cols="50" name="code"></textarea>
    <hr>
    <p>Bit Length: </p><textarea class="console" form="brute" rows="1" cols="50" name="bitlenght"></textarea>
    <hr>
    <INPUT type="radio" name="bruteforce" value="3" hidden="1" checked="checked">
    <INPUT class="submitBtn" type="submit" value="Start">
</form>
<br>
<hr>
<br>
<iframe style="visibility: hidden;" src="http://" )+local_IPstr+"/setrx" name="iframe"></iframe>
</body>
</html>
)=====";
