var websocket = new WebSocket("ws://" + location.hostname + ":81/");

// websocket event declear
websocket.onopen = function (event) {
  onOpen(event);
};
websocket.onclose = function (event) {
  onClose(event);
};
websocket.onerror = function (event) {
  onError(event);
};
websocket.onmessage = function (event) {
  onMessage(event);
};

// websocket events
function onOpen(event) {
  console.log("Server Connected!");
}
function onClose(event) {
  console.log("Server Disconnected!");
  alert("Server Disconnected!");
}
function onError(event) {
  console.log("Error:" + event.data);
  alert("Error Occured!");
}
function onMessage(event) {
  var JSONContent = JSON.parse(event.data);
  if (JSONContent.hasOwnProperty("Door")) {
    document.getElementById("openButton").style.backgroundColor = "white";
  }
}

// Open Door
function openDoor(element) {
  element.style.backgroundColor = "salmon";
  var command = '{"Door":"Open"}';
  websocket.send(command);
  console.log(command);
}
