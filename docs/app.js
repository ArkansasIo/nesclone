(() => {
  const links = document.querySelectorAll('nav a[href^="#"]');
  links.forEach(link => link.addEventListener('click', () => {
    links.forEach(item => item.removeAttribute('aria-current'));
    link.setAttribute('aria-current', 'page');
  }));

  const panel = document.querySelector('#studio-panel');
  const content = {
    library: ['GAME LIBRARY', 'NEX8 Pong, Arcade Lab, NEX8 RPG and SDK samples are organized here as the public game catalog grows.'],
    build: ['BUILD CENTER', 'Use CMake and the Windows scripts to configure, build and test the NEX8 engine, CELL8 model, RAMDB and legacy runtime.'],
    cartridge: ['CARTRIDGE LAB', 'N8CR is the native cartridge container for program, graphics, audio and data sections; N8DB stores RAMDB datasets.'],
    input: ['INPUT CONFIG', 'NEX8 supports keyboard, mouse and gamepad action maps with reusable input profiles for projects.']
  };

  document.querySelectorAll('.dash-card').forEach(card => {
    card.addEventListener('click', () => {
      const value = content[card.dataset.panel];
      if (!value || !panel) return;
      panel.innerHTML = `<strong>${value[0]}</strong><span>${value[1]}</span>`;
      document.querySelectorAll('.dash-card').forEach(item => item.removeAttribute('aria-pressed'));
      card.setAttribute('aria-pressed', 'true');
    });
  });
})();
