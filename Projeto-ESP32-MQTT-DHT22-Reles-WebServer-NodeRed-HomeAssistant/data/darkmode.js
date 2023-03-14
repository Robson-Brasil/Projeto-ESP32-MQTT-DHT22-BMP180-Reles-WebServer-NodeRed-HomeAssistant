document.addEventListener("DOMContentLoaded", function () {

  const modeSwitch = document.getElementById("mode-switch");
  const body = document.querySelector('body');

  modeSwitch.addEventListener('change', function () {
    if (this.checked) {
      body.classList.add('dark-mode');
    } else {
      body.classList.remove('dark-mode');
    }
  });

  const buttons = document.querySelectorAll('button');

  buttons.forEach(button => {
    button.addEventListener('click', function () {
      this.classList.toggle('on');
    });
  });

  function toggleButton(button) {
    var onclick = button.querySelector('input[type="checkbox"]');
    button.classList.toggle("active", onclick.checked);
    if (!onclick.checked) {
      button.classList.remove("active");
    }
  }

  function toggleCheckbox(onclick) {
    var button = onclick.parentNode;
    toggleButton(button);
  }

  function toggleCheckbox(element) {
    var xhr = new XMLHttpRequest();
    if (element.checked) { xhr.open("GET", "/update?relay=" + element.id + "&state=1", true); }
    else { xhr.open("GET", "/update?relay=" + element.id + "&state=0", true); }
    xhr.send();
}

});
