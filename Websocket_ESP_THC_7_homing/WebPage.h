const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.8rem;
    color: white;
  }
  h2{
    font-size: 1.5rem;
    font-weight: bold;
    color: #143642;
  }
  .topnav {
    overflow: hidden;
    background-color: #143642;
  }
  body {
    margin: 0;
  }
  .content {
    padding: 30px;
    max-width: 600px;
    margin: 0 auto;
  }
  .card {
    background-color: #F8F7F9;;
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
    padding-top:10px;
    padding-bottom:20px;
  }
  .button {
    padding: 15px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 5px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
   }

   .button1 {
    padding: 15px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0400ff;
    border: none;
    border-radius: 5px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
   }
   /*.button:hover {background-color: #0f8b8d}*/
   .button:active {
     background-color: #0f8b8d;
     box-shadow: 2 2px #CDCDCD;
     transform: translateY(2px);
   }
   .state {
     font-size: 1.5rem;
     color:#8c8c8c;
     font-weight: bold;
   }
  </style>
<title>ESP Web Server</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body>
  <div class="topnav">
    <h1>ESP WebSocket Server</h1>
  </div>
  <div class="content">
    <div class="card">
      <h2>Output - GPIO 2</h2>
      <p class="state">state: <span id="state">%STATE%</span></p>
      <p><button id="button" class="button">Toggle</button></p>
      <p><button id="lbl_naik" class="button1">NAIK</button></p>
      <p><button id="lbl_stabil" class="button1">STABIL</button></p>
      <p><button id="lbl_turun" class="button1">TURUN</button></p>
    </div>
  </div>
  <br>
  <div class="content">
    <div class="card">
      <h2>HOMING</h2>
      <p><button id="homing" class="button">HOMING</button></p>
    </div>
  </div>
  <br>
  <div class="content">
    <div class="card">
      <h2>Kontrol JOG</h2>
      
      <p><button id="naik" class="button">naik</button></p>
      <p><button id="turun" class="button">turun</button></p>
    </div>
  </div>
  <br>
  
  <div class="content">
    <div class="card">
      <h2>Kontrol THC</h2>
      
      <p><button id="enable" class="button">enable</button></p>
      <p><button id="disable" class="button">disable</button></p>
    </div>
  </div>
  <div class="content">
    <div class="card">
      <h2>Kontrol Titik Nyala</h2>
      <p class="state">Set Point : <span id="set_point">%SET POINT%</span></p>
      <p><button id="step_up" class="button">++</button></p>
      <p><button id="step_down" class="button">--</button></p>
    </div>
  </div>
<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
  }
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onMessage(event) {
    var state;
    let data_masuk = event.data;
    if (data_masuk == "1"){
      state = "ON";
    }
    else{
      state = "OFF";
    }
    if(data_masuk.substring(0,1)=="e"){
        a = data_masuk.indexOf("st")
        console.log(data_masuk.substring(a+2,a+3))
        st = data_masuk.substring(a+2,a+3)
        if(st=="2"){
            document.getElementById('lbl_naik').style="background-color: #f00"
            document.getElementById('lbl_turun').style="background-color: #0400ff"
            document.getElementById('lbl_stabil').style="background-color: #0400ff"
        }else if(st=="1"){
            document.getElementById('lbl_turun').style="background-color: #f00"
            document.getElementById('lbl_naik').style="background-color: #0400ff"
            document.getElementById('lbl_stabil').style="background-color: #0400ff"
        }else if(st=="3"){
            document.getElementById('lbl_stabil').style="background-color: #f00"
            document.getElementById('lbl_turun').style="background-color: #0400ff"
            document.getElementById('lbl_naik').style="background-color: #0400ff"
        }
    }else if(data_masuk.substring(0,1)=="s"){
        document.getElementById('set_point').innerHTML=data_masuk.substring(2,7)
    }
    
    console.log(data_masuk)
    document.getElementById('state').innerHTML = data_masuk;
  }
  function onLoad(event) {
    initWebSocket();
    initButton();
  }
  function initButton() {
    document.getElementById('button').addEventListener('click', toggle);
    document.getElementById('naik').addEventListener('click', naik);
    document.getElementById('turun').addEventListener('click', turun);
    document.getElementById('enable').addEventListener('click', enable);
    document.getElementById('disable').addEventListener('click', disable);
    document.getElementById('step_up').addEventListener('click', step_up);
    document.getElementById('step_down').addEventListener('click', step_down);
    document.getElementById('homing').addEventListener('click', homing);
  }
  function naik() {
      websocket.send('1');
  }
  function turun() {
      websocket.send('0');
  }
  function toggle(){
    websocket.send('toggle');
  }
  function enable(){
    websocket.send('2');
  }
  function disable(){
    websocket.send('3');
  }
  function step_up(){
    websocket.send('a');
  }
  function step_down(){
    websocket.send('b');
  }
  function homing(){
    websocket.send('c');
  }
</script>
</body>
</html>
)rawliteral";
