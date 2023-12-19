const CONTAINER = document.querySelector('.container')
const CARDS = document.querySelectorAll('.article')

var charWidth = 0;
var spaceWidth = 8;
var animSpeed = 40;
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

var text = "Webserver";
var moreText = "Welcome to Webserver, a mini server that hosts your website!";

writeChars("webservTitle", text, 100);
setTimeout(function() {
	writeChars("webservSubtitle", moreText, 300);
}, 500);

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