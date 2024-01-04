const anchor = document.getElementById('anchor');
const loginForm = document.getElementById("loginForm");


anchor.addEventListener('click', function (e) {
	e.preventDefault();

	document.querySelector(this.getAttribute('href')).scrollIntoView({
		behavior: 'smooth'
	});
});

const navigationLoginBtn = document.getElementById('navigationLoginBtn');
const login = document.getElementById('login');

navigationLoginBtn.addEventListener('click', function (e) {
	if(document.cookie.includes("logged_in=true")) {
		const cookies = document.cookie.split(";");
		for (let i = 0; i < cookies.length; i++) {
			const cookie = cookies[i];
			const eqPos = cookie.indexOf("=");
			const name = eqPos > -1 ? cookie.substr(0, eqPos) : cookie;
			document.cookie = name + "=;expires=Thu, 01 Jan 1970 00:00:00 GMT";
		}
		checkLogin();
		return;
	}

	login.classList.toggle('hidden');
	login.style.zIndex = 100;
});

loginForm.addEventListener("submit", e => {
	e.preventDefault();
	const username = document.getElementById("username").value;
	const password = document.getElementById("password").value;

	fetch("/login.php", {
		method: "POST",
		headers: {
			"Content-Type": "application/json"
		},
		body: JSON.stringify({
			username: username,
			password: password
		}),
	}).then((res) => {
		if (res.status === 200) {
			alert("Logged in successfully!");
			document.cookie = "logged_in=true";
			document.cookie = "username=" + username;
			checkLogin();
		} else {
			alert("Incorrect username or password.");
		}
	}).catch((error) => {
		console.log(error);
	});
});

function checkLogin() {

	if (document.cookie.includes("logged_in=true")) {
		switchPage("logged_in");
	} else {
		switchPage("not_logged_in");
	}
}

function switchPage(page) {
	const navigationLoginText = document.getElementById("navigationLoginText");

	switch (page) {
		case "not_logged_in":
			navigationLoginText.innerHTML = "Login to<br>your account";
			break;
		case "logged_in":
			navigationLoginText.innerHTML = "Logged in as<br>" + document.cookie.split("username=")[1];
			login.classList.add('hidden');
			break;
	}
}

checkLogin();

// const uploadBtn = document.getElementById('uploadBtn');

// uploadBtn.addEventListener('click', function (e) {
// 	const upload = document.getElementById('upload');

// 	upload.classList.toggle('hidden');
// 	upload.style.zIndex = 100;
// });