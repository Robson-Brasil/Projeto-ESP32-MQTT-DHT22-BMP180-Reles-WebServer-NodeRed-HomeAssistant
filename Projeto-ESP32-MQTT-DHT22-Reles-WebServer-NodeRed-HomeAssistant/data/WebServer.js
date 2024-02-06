document.addEventListener('DOMContentLoaded', () => {
    const darkModeToggle = document.getElementById('darkModeToggle');
    const switchButtons = document.querySelectorAll('.switch-button');

    // Recupera o estado do dark mode do LocalStorage
    const darkModeState = localStorage.getItem('darkModeState');
    if (darkModeState === 'enabled') {
        document.body.classList.add('dark-mode');
        darkModeToggle.checked = true;
    }

    darkModeToggle.addEventListener('change', () => {
        // Atualiza o estado do dark mode no LocalStorage
        if (darkModeToggle.checked) {
            document.body.classList.add('dark-mode');
            localStorage.setItem('darkModeState', 'enabled');
        } else {
            document.body.classList.remove('dark-mode');
            localStorage.setItem('darkModeState', 'disabled');
        }
    });

    switchButtons.forEach(button => {
        button.addEventListener('click', () => {
            button.classList.toggle('active');

            // Adicione aqui a lógica para armazenar o estado do botão no LocalStorage
            const buttonId = button.id;
            const buttonState = button.classList.contains('active') ? '1' : '0';
            localStorage.setItem(buttonId, buttonState);

            // Adicione aqui a lógica para enviar a alteração do estado ao servidor
            updateButtonStateOnServer(buttonId, buttonState);
        });

        // Recupera o estado do botão do LocalStorage e aplica ao carregar a página
        const buttonId = button.id;
        const buttonState = localStorage.getItem(buttonId);
        if (buttonState === '1') {
            button.classList.add('active');
        }
    });

    // Função para enviar a alteração do estado ao servidor
    function updateButtonStateOnServer(buttonId, buttonState) {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/update?relay=" + buttonId + "&state=" + buttonState, true);
        xhr.send();
    }

    // Função para atualizar o estado dos botões ao carregar a página
    function updateButtonStates() {
        var xhr = new XMLHttpRequest();
        xhr.onreadystatechange = function () {
            if (xhr.readyState == 4 && xhr.status == 200) {
                var buttonStates = JSON.parse(xhr.responseText);

                // Atualiza o estado dos botões na página
                for (var buttonId in buttonStates) {
                    var button = document.getElementById(buttonId);
                    if (button) {
                        button.classList.toggle('active', buttonStates[buttonId] === '1');
                    }
                }
            }
        };

        xhr.open("GET", "/getButtonStates", true);
        xhr.send();
    }

    // Atualiza o estado dos botões ao carregar a página
    updateButtonStates();
});