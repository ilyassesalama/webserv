const anchor = document.getElementById('anchor');

anchor.addEventListener('click', function (e) {
	e.preventDefault();

	document.querySelector(this.getAttribute('href')).scrollIntoView({
		behavior: 'smooth'
	});
});