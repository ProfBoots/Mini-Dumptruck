// This file contains the HTML data for the ESP32.

const char* htmlHomePage PROGMEM = R"HTMLHOMEPAGE(
<!DOCTYPE html>
<html>
  <head>
  <meta name="viewport" content="width=device-width, initial-scale=.9, maximum-scale=1, user-scalable=yes">
  <style>
    .arrows {
      font-size:50px;
      color:grey;
    }
    td.button {
      background-color:black;
      border-radius:20%;
      box-shadow: 5px 5px #888888;
    }
    td.button:active {
      transform: translate(5px,5px);
      box-shadow: none; 
    }
    .auxButton {
  background-color: black; /* Background color of the button */
  box-shadow: 5px 5px #888888;
  color: grey; /* Text color */
  padding: 30px 35px; /* Padding for the button */
  border: none; /* Remove the button border */
  border-radius: 20%; /* Rounded corners */
  font-size: 24px; /* Font size */
  cursor: pointer; /* Cursor style on hover */
  transform: rotate(90deg); /* Rotate the text vertically */
  transform-origin: left center; /* Adjust the origin to change the rotation pivot */
  margin-left: 60px;
  margin-top: 55px;
}
    .auxButton:active {
      transform: translate(-5px,5px);
      transform: rotate(90deg); /* Rotate the text vertically */
      transform-origin: left center;
      
      box-shadow: none; 
    }

    .noselect {
      -webkit-touch-callout: none; /* iOS Safari */
        -webkit-user-select: none; /* Safari */
         -khtml-user-select: none; /* Konqueror HTML */
           -moz-user-select: none; /* Firefox */
            -ms-user-select: none; /* Internet Explorer/Edge */
                user-select: none; /* Non-prefixed version, currently
                                      supported by Chrome and Opera */
    }
        .slidecontainer {
      width: 100%;
    }

    .slider {
      -webkit-appearance: none;
      width: 150%;
      height: 20px;
      border-radius: 5px;
      background: #d3d3d3;
      outline: none;
      opacity: 0.7;
      -webkit-transition: .2s;
      transition: opacity .2s;
    }

    .slider:hover {
      opacity: 1;
    }
  
    .slider::-webkit-slider-thumb {
      -webkit-appearance: none;
      appearance: none;
      width: 65px;
      height: 65px;
      border-radius: 50%;
      background: red;
      cursor: pointer;
    }

    .slider::-moz-range-thumb {
      width: 60px;
      height: 40px;
      border-radius: 50%;
      background: red;
      cursor: pointer;
    }

.vertical-slider-container {
  display: flex;
  flex-direction: column;
  align-items: center;
  margin-top: 165px;
  width: 5px; /* Adjust the width as needed */
  height: 100px; /* Adjust the height as needed */
}

.vertical-slider {
  writing-mode: bt-lr; /* IE/Edge specific property for vertical text */
  -webkit-appearance: none;
  width: 400px;
  height: 20px;
  transform: rotate(270deg);
     background: #d3d3d3; /* Background color of the slider */
        outline: none;
      opacity: 0.7;
      -webkit-transition: .2s;
      transition: opacity .2s;
}

.vertical-slider:hover {
      opacity: 1;
    }
.vertical-slider::-webkit-slider-thumb {
        -webkit-appearance: none;
      appearance: none;
  width: 65px; /* Adjust the width to make the slider thumb thicker */
  height: 65px; /* Adjust the height to make the slider thumb thicker */
  background-color: red; /* Background color of the slider thumb */
  border: none; /* Remove the default border */
  //margin-top: -5px; /* Center the thumb vertically within the track */
}
    .vertical-slider::-moz-range-thumb {
      width: 60px;
      height: 40px;
      border-radius: 50%;
      background: red;
      cursor: pointer;
    }

    </style>
  
  </head>
  <body class="noselect" align="center" style="background-color:white; overflow: hidden;" >
    <h1 style="color: black; text-align:center;">MINI-DUMP</h1>
    
    <table id="mainTable" style="width:400px;margin:auto;table-layout:fixed" CELLSPACING=10>
    <tr>
        <td colspan=2 style="text-align: center;">
         <div class="slidecontainer">
            <input type="range" min="-255" max="255" value="0" class="slider" id="throttle" oninput='sendButtonInput("throttle",value)' ontouchend='resetSlider("throttle")'>
          </div>
        </td>
      </tr>  
      <tr/>
      <tr/>
      <tr/><tr/>
      <tr>
        <td class="button"
    ontouchstart='sendButtonInput("dump", "5")'
    onmousedown='sendButtonInput("dump", "5")'
    ontouchend='sendButtonInput("dump", "0")'
    onmouseup='sendButtonInput("dump", "0")'>
    <span class="arrows">&#8678;</span></td>
        <td class="button" ontouchstart='sendButtonInput("light","6")'onmousedown='sendButtonInput("light","6")'onmouseup='sendButtonInput("MoveCar","0")' ontouchend='sendButtonInput("MoveCar","0")'><span class="arrows" >&#9788;</span></td>   
        <td class="button"
    ontouchstart='sendButtonInput("dump", "6")'
    onmousedown='sendButtonInput("dump", "6")'
    ontouchend='sendButtonInput("dump", "0")'
    onmouseup='sendButtonInput("dump", "0")'>
    <span class="arrows">&#8680;</span></td>
      </tr>
      <tr/>
      <tr/>
      <tr/><tr/>
<tr>
  <td style="text-align: left; font-size: 25px"><b></b></td>
  <td>
    <div class="vertical-slider-container">
      <input type="range" min="40" max="132" value="86" class="vertical-slider" id="steering" oninput='sendButtonInput("steering", value)'ontouchend='resetSlider("steering")'>
    </div>
  </td>
  <td>
    <button id="auxButton" class="auxButton"
    ontouchstart='startSendingButtonInput("trim", "1")'
    onmousedown='startSendingButtonInput("trim", "1")'
    ontouchend='stopSendingButtonInput()'
    onmouseup='stopSendingButtonInput()'>LTRIM</button>
    <button id="auxButton" class="auxButton"
    ontouchstart='startSendingButtonInput("trim", "2")'
    onmousedown='startSendingButtonInput("trim", "2")'
    ontouchend='stopSendingButtonInput()'
    onmouseup='stopSendingButtonInput()'>RTRIM</button>
</td>
<tr/>
<tr/>
</tr>
</tr>
    </table>
  
    <script>
      var webSocketCarInputUrl = "ws:\/\/" + window.location.hostname + "/CarInput";      
      var websocketCarInput;
      const throttleSlider = document.getElementById('throttle');
      const steeringSlider = document.getElementById('steering');

      function resetSlider(sliderId) 
      {
       var slider = document.getElementById(sliderId);
       var middleValue = (parseInt(slider.min) + parseInt(slider.max)) / 2;
       slider.value = middleValue;
       sendButtonInput(sliderId, middleValue);
      }
      
      function initCarInputWebSocket() 
      {
        websocketCarInput = new WebSocket(webSocketCarInputUrl);
        websocketCarInput.onclose   = function(event){setTimeout(initCarInputWebSocket, 2000);};
        websocketCarInput.onmessage = function(event){};        
      }
      
      function sendButtonInput(key, value) 
      {
       var data = key + "," + value;
       websocketCarInput.send(data);
      }
      let intervalId = null;

    function startSendingButtonInput(action, value) {
    sendButtonInput(action, value); // Send the initial input when the button is pressed
    intervalId = setInterval(function() {
        sendButtonInput(action, value); // Continuously send the input as long as the button is pressed
    }, 10); // You can adjust the interval (in milliseconds) to control the rate of sending
    }

    function stopSendingButtonInput() {
    clearInterval(intervalId); // Stop sending the input when the button is released
}
      function handleKeyDown(event) {
        if (event.keyCode ===88)
        {
          sendButtonInput("light", "1");
        }
        if (event.keyCode === 81)
        {
          sendButtonInput("dump", "5");
        }
        if (event.keyCode === 69)
        {
          sendButtonInput("dump", "6");
        }
        if(event.keyCode === 87)
        {
          throttleSlider.value = parseInt(throttleSlider.value) + 255; // You can adjust the increment value as needed
          sendButtonInput("throttle",throttleSlider.value);
      // Trigger the 'input' event on the slider to update its value
          throttleSlider.dispatchEvent(new Event('input'));
        }
        if(event.keyCode === 83)
        {
          throttleSlider.value = parseInt(throttleSlider.value) - 255; // You can adjust the increment value as needed
          sendButtonInput("throttle",throttleSlider.value);
      // Trigger the 'input' event on the slider to update its value
          throttleSlider.dispatchEvent(new Event('input'));
        }
        if(event.keyCode === 65)
        {
          steeringSlider.value = parseInt(steeringSlider.value) + 30; // You can adjust the increment value as needed
          sendButtonInput("steering",steeringSlider.value);
      // Trigger the 'input' event on the slider to update its value
          steeringSlider.dispatchEvent(new Event('input'));
        }
        if(event.keyCode === 68)
        {
          steeringSlider.value = parseInt(steeringSlider.value) - 30; // You can adjust the increment value as needed
          sendButtonInput("steering",steeringSlider.value);
      // Trigger the 'input' event on the slider to update its value
          steeringSlider.dispatchEvent(new Event('input'));
        }
        }  
      
  
      window.onload = initCarInputWebSocket;
      document.getElementById("mainTable").addEventListener("touchend", function(event){
        event.preventDefault()
      });
      document.addEventListener('keydown', handleKeyDown);
      document.addEventListener('keyup', handleKeyUp); 
           
    </script>
  </body>    
</html>
)HTMLHOMEPAGE";
