const char CSSStyle[] PROGMEM = R"=====(
body {
background-color: #888;
font-family: "Century Gothic", Arial;
color: white
}

body {
background: linear-gradient(45deg, #444 10%, #777 47%, #222 92%);
background-size: 200% 200%;
animation: gradient 20s ease-in-out infinite
}

iframe {
border: 0px;
width: 100%
}

nav {
margin: 20px 10px 0px 10px
}

nav ul {
background: linear-gradient(#777, #444);
padding: 0px;
list-style: none;
position: relative
}

nav ul li {
display: inline-block;
background: linear-gradient(#777, #444);
Z-Index: 9999
}

nav a {
display: block;
padding: 0 10px;
color: #FFF;
font-size: 16px;
line-height: 40px;
text-decoration: none;
border: 2px solid #777
}

nav a:hover {
opacity: 0.7
}

nav ul ul {
display: none;
position: absolute
}

nav ul li:hover {
background: grey;
color: black
}

nav ul li:hover>ul {
border: 1px solid lightgrey;
display: inherit;
Z-Index: 9999
}

nav ul ul li {
width: 250px;
float: none;
display: list-item;
position: relative;
Margin: 1px
}

option {
font-size: 110% !important
}

.slider {
-webkit-appearance: none;
appearance: none;
background: #777;
opacity: .7;
-webkit-transition: .2s;
transition: opacity .2s
}

.slider::-webkit-slider-thumb {
-webkit-appearance: none;
appearance: none;
width: 25px;
height: 25px;
background: #444;
cursor: pointer
}

.slider:hover::-webkit-slider-thumb {
box-shadow: 0px 0px 0px 2px deepskyblue;
-webkit-transition: .3s;
transition: .3s
}

.slider:hover,
.console:hover {
opacity: 1
}

.container {
display: grid;
grid-column-gap: 10px
}

.setting {
display: grid;
grid-column-gap: 10px;
grid-template-columns: 1fr 1fr 4fr 4fr;
background: linear-gradient(#777, #444);
color: white;
padding: 0px 10px
}

.setting a {
color: white;
align-self: center;
}
.setting p {
overflow: hidden;
color: white;
align-self: center;
}
.setting select {
color: white;
align-self: center;
}

@media (max-width:1480px) {
.setting {
grid-template-columns: 1fr 1fr 4fr 4fr;
height: 60px;
}
}
@media (max-width:800px) {
.setting {
grid-template-columns: 1.3fr 1fr 3.7fr;
height: 55px;
}
.break {
grid-column-start: 1;
grid-column-end: 6;
visibility: hidden;
display: none
}
}

.Btn,
.Btn:link,
.Btn:visited {
text-decoration: none;
display: inline-block;
background: linear-gradient(#777, #444);
color: white;
padding: 4px;
min-width: 100px;
border-radius: 2.5px;
border: 2px solid white;
text-align: center
}

.Btn:hover,
.Btn:active {
background: linear-gradient(#888, #666);
border: 2px solid deepskyblue;
Margin: -2 px
}

.log {
width: 100%;
Height: 200px;
margin: 5px 0px;
background-color: #999;
color: white;
font-family: monospace;
overflow-y: scroll
}

.console {
width: 100%;
Height: 35px;
font-size: 130%;
opacity: .7;
background-color: #777;
color: white;
font-family: monospace;
padding: 0px;
border: 0;
align-self: center;
justify-self: center
}

cs {
grid-column-start: 1;
grid-column-end: 2;
-ms-grid-column-span: 2
}

::-webkit-scrollbar {
width: 2em;
height: 2em;
}

::-webkit-scrollbar-button {
height: 0em;
}

::-webkit-scrollbar-track-piece {
background: #999;
}

::-webkit-scrollbar-thumb {
background: #ddd;
}
)=====";
