const CONTAINER = document.querySelector('.container')
const CARDS = document.querySelectorAll('.article')

const pageTitle = document.getElementById("webservTitle");
const pageSubtitle = document.getElementById("webservSubtitle");
const loggedInTitle = document.getElementById("loggedInTitle");

const homePage = document.getElementById("home");
const loginPage = document.getElementById("login");
const uploadPage = document.getElementById("upload");

const loginForm = document.getElementById("loginForm");

const homeNavButton = document.getElementById("homeNavButton");
const loginNavButton = document.getElementById("loginNavButton");
const uploadNavButton = document.getElementById("uploadNavButton");

loginNavButton.addEventListener("click", () => {
	checkLogin();
});

homeNavButton.addEventListener("click", () => {
	checkLogin();
});

uploadNavButton.addEventListener("click", () => {
	switchPage("upload");
});

const fileLabel = document.getElementById("file-label");
const inputFile = document.getElementById("file");

inputFile.addEventListener("change", () => {
	const file = inputFile.files[0];

	fileLabel.innerHTML = file.name;
})

function switchPage(page){
	var titleText = "Webserver";
	var subText = "Welcome to Webserver, a mini server that hosts your website!";
	document.getElementById("webservTitle").innerHTML = "";
	document.getElementById("webservSubtitle").innerHTML = "";
	switch (page) {
		case "home":
			homePage.classList.add("active");
			loginPage.classList.remove("active");
			uploadPage.classList.remove("active");
			homeNavButton.style.display = "none";
			loginNavButton.style.display = "block";
			uploadNavButton.style.display = "block";
			break;
		case "login":
			homePage.classList.remove("active");
			loginPage.classList.add("active");
			uploadPage.classList.remove("active");
			homeNavButton.style.display = "block";
			loginNavButton.style.display = "none";
			uploadNavButton.style.display = "block";
			titleText = "Login";
			subText = "Login using your username and password to continue.";
			break;
		case "upload":
			homePage.classList.remove("active");
			loginPage.classList.remove("active");
			uploadPage.classList.add("active");
			homeNavButton.style.display = "block";
			loginNavButton.style.display = "block";
			uploadNavButton.style.display = "none";
			titleText = "Upload";
			subText = "Upload your website to the server.";
			break;
		case "logged_in":
			homePage.classList.add("active");
			loginPage.classList.remove("active");
			uploadPage.classList.remove("active");
			homeNavButton.style.display = "none";
			loginNavButton.style.display = "none";
			uploadNavButton.style.display = "block";
			titleText = "Logged In";
			subText = "Welcome back, " + document.cookie.split("username=")[1] + " to our awesome webserver!"
			break;
	}
	setTimeout(function() {
		writeChars("webservTitle", titleText, 200);
	}, 50);
	setTimeout(function() {
		writeChars("webservSubtitle", subText, 300);
	}, 300);
}

const loginBtn = document.getElementById("login-btn");

loginForm.addEventListener("submit", e => {
	e.preventDefault();
	const username = document.getElementById("username").value;
	const password = document.getElementById("password").value;

	fetch("./login.php", {
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
		switchPage("home");
	}
}

checkLogin();

// --------------------- text animation stuff --------------------- //

var charWidth = 0;
var spaceWidth = 8;
var animSpeed = 60;
var fadeSec = 0.5;
var lineSpace = 25;

function writeChars(p, t, lim) {
	var zone = document.getElementById(p);
	var width = 0;
	var top = 0;
	var chars = t.split("");
	for (var i = 0; i < chars.length; i++) {
		var s = "<span id ='" + p + "char" + i +
				"' class='writer' style='top:" + top + "px; left:" + width + 
				"px;animation:charAnim " + fadeSec + "s linear " + i/animSpeed + "s forwards;'>"
				+ chars[i] + "</span>";
		var node = document.createElement("div");
		node.innerHTML = s;
		zone.appendChild(node);
		var blah = document.getElementById(p + "char" + i);
		if (blah.offsetWidth == 0) {
			width += spaceWidth;
		}
		else {
			width += blah.offsetWidth+charWidth;
		}
		if (width >= lim && blah.offsetWidth == 0) {
			width = 0;
			top += lineSpace;
		}
	}
}
// --------------------- glowing box stuff --------------------- //

const CONFIG = {
  proximity: 40,
  spread: 80,
  blur: 20,
  gap: 32,
  vertical: false,
  opacity: 0,
}

const PROXIMITY = 10

const UPDATE = (event) => {
  // get the angle based on the center point of the card and pointer position
  for (const CARD of CARDS) {
    // Check the card against the proximity and then start updating
    const CARD_BOUNDS = CARD.getBoundingClientRect()
    // Get distance between pointer and outerbounds of card
    if (
      event?.x > CARD_BOUNDS.left - CONFIG.proximity &&
      event?.x < CARD_BOUNDS.left + CARD_BOUNDS.width + CONFIG.proximity &&
      event?.y > CARD_BOUNDS.top - CONFIG.proximity &&
      event?.y < CARD_BOUNDS.top + CARD_BOUNDS.height + CONFIG.proximity) {
      // If within proximity set the active opacity
      CARD.style.setProperty('--active', 1)
    } else {
      CARD.style.setProperty('--active', CONFIG.opacity)
    }
    const CARD_CENTER = [
      CARD_BOUNDS.left + CARD_BOUNDS.width * 0.5,
      CARD_BOUNDS.top + CARD_BOUNDS.height * 0.5
    ]
    let ANGLE = Math.atan2(event?.y - CARD_CENTER[1], event?.x - CARD_CENTER[0]) * 180 / Math.PI
    ANGLE = ANGLE < 0 ? ANGLE + 360 : ANGLE;
    CARD.style.setProperty('--start', ANGLE + 90)
  }

}

document.body.addEventListener('pointermove', UPDATE)

const RESTYLE = () => {
  CONTAINER.style.setProperty('--gap', CONFIG.gap)
  CONTAINER.style.setProperty('--blur', CONFIG.blur)
  CONTAINER.style.setProperty('--spread', CONFIG.spread)
  CONTAINER.style.setProperty('--direction', CONFIG.vertical ? 'column' : 'row')
}

const CTRL = new GUI({
  width: 340,
})
CTRL.add(CONFIG, 'spread', 10, 180, 1).name('Spread (deg)').onChange(RESTYLE)
CTRL.add(CONFIG, 'proximity', 10, 180, 1).name('Active Proximity (px)').onChange(RESTYLE)
CTRL.add(CONFIG, 'gap', 10, 100, 1).name('Gap (px)').onChange(RESTYLE)
CTRL.add(CONFIG, 'blur', 0, 50, 1).name('Blur (px)').onChange(RESTYLE)
CTRL.add(CONFIG, 'opacity', 0, 1, 0.01).name('Inactive Opacity').onChange(RESTYLE)
CTRL.add(CONFIG, 'vertical').name('Vertical Layout').onChange(RESTYLE)

RESTYLE()
UPDATE()