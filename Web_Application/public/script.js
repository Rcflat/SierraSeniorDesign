document.addEventListener('DOMContentLoaded', function () {
    const fileSystemElement = document.getElementById('file-system');
    const logPanel = document.getElementById('log');
    const gamepadStatus = document.getElementById('gamepad-status');
    const gamepadDisplay = document.getElementById('gamepad-display');

    console.log("DOM fully loaded and parsed");

    function fetchFileSystem() {
        console.log("Fetching file system data...");
        fetch('/files')
            .then(response => response.json())
            .then(data => {
                console.log("File system data received:", data);
                fileSystemElement.innerHTML = generateFileSystemHtml(data);
                addEventListeners();
            })
            .catch(error => {
                console.error("Error fetching file system data:", error);
                logPanel.innerHTML += `<div>Error fetching file system data: ${error.message}</div>`;
            });
    }

    function generateFileSystemHtml(files) {
        console.log("Generating HTML for file system...");
        let html = '<ul>';
        files.forEach(file => {
            if (file.isDirectory) {
                console.log("Processing directory:", file.name);
                html += `<li><span class="directory">${file.name}</span>${generateFileSystemHtml(file.children)}</li>`;
            } else {
                console.log("Processing file:", file.name);
                html += `<li><span class="file">${file.name}</span></li>`;
            }
        });
        html += '</ul>';
        return html;
    }

    function addEventListeners() {
        console.log("Adding event listeners to directories and files...");
        const directories = document.querySelectorAll('.directory');
        directories.forEach(directory => {
            directory.addEventListener('click', function () {
                const sublist = this.nextElementSibling;
                if (sublist) {
                    sublist.style.display = sublist.style.display === 'none' ? 'block' : 'none';
                    console.log("Toggled display for directory:", this.textContent);
                }
                logPanel.innerHTML += `<div>Clicked on directory: ${this.textContent}</div>`;
            });
        });

        const files = document.querySelectorAll('.file');
        files.forEach(file => {
            file.addEventListener('click', function () {
                console.log("Clicked on file:", this.textContent);
                logPanel.innerHTML += `<div>Clicked on file: ${this.textContent}</div>`;
            });
        });
    }

    async function startCamoFeed() {
        console.log("Starting Camo feed...");
        try {
            const stream = await navigator.mediaDevices.getUserMedia({
                video: { deviceId: 'Camo Camera' } // Replace with the actual label for your Camo camera
            });
            const videoElement = document.getElementById('video-feed');
            videoElement.srcObject = stream;
            console.log("Camo feed started successfully.");
        } catch (error) {
            console.error("Error accessing Camo camera:", error);
            logPanel.innerHTML += `<div>Error accessing Camo camera: ${error.message}</div>`;
        }
    }

    // Initialize all components
    fetchFileSystem();
    startCamoFeed();
    setupGamepadDisplay(); // Setup the gamepad display functionality
});

function toggleController() {
    console.log("Toggling controller overlay...");
    const overlay = document.getElementById('controller-overlay');
    const button = document.getElementById('toggle-button');

    if (overlay.classList.contains('hidden')) {
        overlay.classList.remove('hidden');
        button.textContent = 'Hide Controller';
        console.log("Controller overlay shown.");
    } else {
        overlay.classList.add('hidden');
        button.textContent = 'Show Controller';
        console.log("Controller overlay hidden.");
    }
}

function setupGamepadDisplay() {
    console.log("Setting up gamepad display...");

    window.addEventListener("gamepadconnected", function (e) {
        console.log("Gamepad connected:", e.gamepad);
        gamepadStatus.textContent = `Gamepad connected: ${e.gamepad.id}`;
        findActiveGamepad();
    });

    window.addEventListener("gamepaddisconnected", function (e) {
        console.log("Gamepad disconnected:", e.gamepad);
        gamepadStatus.textContent = "Gamepad disconnected.";
        gamepadDisplay.innerHTML = ""; // Clear the display when disconnected
    });

    function findActiveGamepad() {
        const gamepads = navigator.getGamepads();
        for (let i = 0; i < gamepads.length; i++) {
            if (gamepads[i]) { // Check for a connected gamepad
                console.log("Using gamepad:", gamepads[i]);
                updateGamepadDisplay(gamepads[i]);
                break; // Exit loop after finding the first active gamepad
            }
        }
    }

    function updateGamepadDisplay(gamepad) {
        console.log("Updating gamepad display for gamepad:", gamepad);
        requestAnimationFrame(function update() {
            const gamepads = navigator.getGamepads();
            if (!gamepads[gamepad.index]) {
                console.log("Gamepad disconnected, stopping update.");
                return; // Exit if the gamepad is disconnected
            }

            const gp = gamepads[gamepad.index];
            console.log("Gamepad data:", gp);

            let displayHtml = '<div>Axes: ' + gp.axes.map(axis => axis.toFixed(2)).join(', ') + '</div>';
            displayHtml += '<div>Buttons: ' + gp.buttons.map((button, index) => `Button ${index}: ${button.pressed ? 'Pressed' : 'Released'}`).join(', ') + '</div>';

            gamepadDisplay.innerHTML = displayHtml; // Update the display with gamepad data
            console.log("Gamepad display updated:", displayHtml);
            requestAnimationFrame(update); // Keep updating as long as the gamepad is connected
        });
    }

    findActiveGamepad(); // Check for gamepads on load
}


