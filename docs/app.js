(() => {
  const links = document.querySelectorAll('nav a[href^="#"]');
  links.forEach(link => link.addEventListener('click', () => {
    links.forEach(item => item.removeAttribute('aria-current'));
    link.setAttribute('aria-current', 'page');
  }));
})();
