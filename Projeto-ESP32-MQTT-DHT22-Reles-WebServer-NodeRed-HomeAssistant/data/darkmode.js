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
            const buttonState = localStorage.getItem(buttonId);
            localStorage.setItem(buttonId, buttonState === '1' ? '0' : '1');
        });

        // Recupera o estado do botão do LocalStorage e aplica ao carregar a página
        const buttonId = button.id;
        const buttonState = localStorage.getItem(buttonId);
        if (buttonState === '1') {
            button.classList.add('active');
        }
    });
});
