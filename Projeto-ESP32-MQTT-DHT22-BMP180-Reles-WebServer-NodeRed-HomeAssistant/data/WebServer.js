document.addEventListener('DOMContentLoaded', () => {
    const WebServerToggle = document.getElementById('WebServerToggle');
    const switchButtons = document.querySelectorAll('.switch-button input[type="checkbox"]');

    // Recupera o estado do dark mode do LocalStorage
    const WebServerState = localStorage.getItem('WebServerState');
    if (WebServerState === 'enabled') {
        document.body.classList.add('dark-mode');
        WebServerToggle.checked = true;
    }

    WebServerToggle.addEventListener('change', () => {
        // Atualiza o estado do dark mode no LocalStorage
        if (WebServerToggle.checked) {
            document.body.classList.add('dark-mode');
            localStorage.setItem('WebServerState', 'enabled');
        } else {
            document.body.classList.remove('dark-mode');
            localStorage.setItem('WebServerState', 'disabled');
        }
    });

    switchButtons.forEach(button => {
        const buttonId = button.id;
        const buttonState = localStorage.getItem(buttonId);

        if (buttonState === '1') {
            button.checked = true;
        }

        button.addEventListener('change', () => {
            const newButtonState = button.checked ? '1' : '0';
            localStorage.setItem(buttonId, newButtonState);
            updateButtonStateOnServer(buttonId, newButtonState);
        });
    });

    // Função para enviar a alteração do estado ao servidor
    function updateButtonStateOnServer(buttonId, buttonState) {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/update?relay=" + buttonId + "&state=" + buttonState, true);
        xhr.send();
    }

    // WebSocket para receber atualizações de estado dos botões do servidor MQTT
    const gateway = `ws://192.168.10.10:8080/ws`;
    let websocket;

    function initWebSocket() {
        console.log('Trying to open a WebSocket connection…');
        websocket = new WebSocket(gateway);
        websocket.onopen = onOpen;
        websocket.onclose = onClose;
        websocket.onmessage = onMessage;
    }

    function getReadings(){
        websocket.send("getReadings");
    }

    // When websocket is established, call the getReadings() function
    function onOpen(event) {
        console.log('Connection opened');
        getReadings();
    }

    function onClose(event) {
        console.log('Connection closed');
        setTimeout(initWebSocket, 2000);
    }

    // Function that receives the message from the ESP32 with the readings
    function onMessage(event) {
        console.log(event.data);
        const myObj = JSON.parse(event.data);
        const keys = Object.keys(myObj);

        keys.forEach(key => {
            const element = document.getElementById(key);
            if (element) {
                element.innerHTML = myObj[key];
            }
        });
    }
});