document.addEventListener('DOMContentLoaded', () => {
    const WebServerToggle = document.getElementById('WebServerToggle');
    const switchButtons = document.querySelectorAll('.switch-button');

    WebServerToggle.addEventListener('change', () => {
        document.body.classList.toggle('dark-mode');
    });

    switchButtons.forEach(button => {
        button.addEventListener('click', () => {
            button.classList.toggle('active');
        });
    });
});