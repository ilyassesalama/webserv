const anchor = document.getElementById('anchor');
const loginForm = document.getElementById("loginForm");
const navigationLoginText = document.getElementById("navigationLoginText");
const navigationLoginSubtitle = document.getElementById("navigationLoginSubtitle");
const loginpage__title = document.getElementById("loginpage__title");


anchor.addEventListener('click', function (e) {
	e.preventDefault();

	document.querySelector(this.getAttribute('href')).scrollIntoView({
		behavior: 'smooth'
	});
});

const navigationLoginBtn = document.getElementById('navigationLoginBtn');
const login = document.getElementById('login');

navigationLoginBtn.addEventListener('click', function (e) {
	if (isLoggedIn()) {
		logout();
		return;
	}
	if(tryingToLogin())
		switchLoginPage("login_default");
	else
		switchLoginPage("trying_to_login");	
});

loginForm.addEventListener("submit", e => {
	e.preventDefault();
	const username = document.getElementById("username").value;
	const password = document.getElementById("password").value;

	loginpage__title.innerHTML = "Logging in...";
	document.getElementById("login-btn").disabled = true; // to avoid making the cool effect shit

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
		setTimeout(() => { // for the cool effect
			document.getElementById("login-btn").disabled = false;

			if (res.status === 200) {
				document.cookie = "logged_in=true";
				document.cookie = "username=" + username;
				loginpage__title.innerHTML = "Login";
				switchLoginPage("logged_in");
			} else {
				triggerShakeAnimation(loginpage__title);
				loginpage__title.innerHTML = "Wrong username or password";
				setTimeout(() => {
					changeTextWithAnimation(loginpage__title, "Login");
				}, 2000);
			}
		}, 800);
	}).catch((error) => {
		console.log(error);
	});
});

function switchLoginPage(page, showLogin = true) {
	const navigationLoginText = document.getElementById("navigationLoginText");
	console.log(page);
	switch (page) {
		case "login_default":
			navigationLoginText.innerHTML = "Login to<br>your account";
			navigationLoginSubtitle.innerHTML = "Click here to login to your useless account and do nothing";
			break;
		case "logged_in":
			navigationLoginText.innerHTML = "Logged in as<br>\"" + document.cookie.split("username=")[1] + "\"";
			navigationLoginSubtitle.innerHTML = "You can always click here to log out";
			break;
		case "trying_to_login":
			navigationLoginText.innerHTML = "Trying to<br>login...";
			navigationLoginSubtitle.innerHTML = "Ask the team for the username and the password";
			break;
		case "logged_out":
			navigationLoginText.innerHTML = "Logged out<br>successfully";
			navigationLoginSubtitle.innerHTML = "Want to login again? Click here";
			break;
		}
	if(showLogin){
		login.classList.toggle('hidden');
		login.style.zIndex = 100;
	
	}
}

function checkInitialLogin() {
	if (isLoggedIn()) {
		switchLoginPage("logged_in", false);
	} else {
		switchLoginPage("login_default", false);
	}
}

function logout() {
	navigationLoginText.innerHTML = "Logged out<br>successfully";
	navigationLoginSubtitle.innerHTML = "Click here to login to your useless account and do nothing";
	const cookies = document.cookie.split(";");

    for (let i = 0; i < cookies.length; i++) {
        const cookie = cookies[i];
        const eqPos = cookie.indexOf("=");
        const name = eqPos > -1 ? cookie.substr(0, eqPos) : cookie;
        document.cookie = name + "=;expires=Thu, 01 Jan 1970 00:00:00 GMT";
    }
	switchLoginPage("logged_out");
}

function tryingToLogin() {
	return navigationLoginText.innerHTML == "Trying to<br>login...";
}

function isLoggedIn() {
	return document.cookie.includes("logged_in=true");
}

checkInitialLogin();








// miscllaneous functions

function triggerShakeAnimation(element) {
    element.classList.add("shake-animation");

    // Remove the class after the animation completes
    element.addEventListener('animationend', function() {
        element.classList.remove("shake-animation");
    }, {once: true});
}

function changeTextWithAnimation(element, newText) {
    // Step 1: Slide left and fade out
    element.classList.add("slide-left-fade-out");

    // Step 2: Once the fade-out animation is complete, change the text and slide in
    element.addEventListener('animationend', function() {
        // Remove the old animation class
        element.classList.remove("slide-left-fade-out");

        // Change the text
        element.textContent = newText;

        // Add the new animation class
        element.classList.add("slide-right-fade-in");

        // Clean up after the slide-in animation is complete
        element.addEventListener('animationend', function() {
            element.classList.remove("slide-right-fade-in");
        }, { once: true });
    }, { once: true });
}
