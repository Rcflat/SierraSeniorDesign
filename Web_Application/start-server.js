const { exec } = require('child_process');
const path = require('path');

// Path to your Node.js application entry file
const serverPath = path.join(__dirname, 'server.js');

// Start the server
exec(`node ${serverPath}`, (error, stdout, stderr) => {
    if (error) {
        console.error(`Error: ${error.message}`);
        return;
    }
    if (stderr) {
        console.error(`stderr: ${stderr}`);
        return;
    }
    console.log(`stdout: ${stdout}`);
});