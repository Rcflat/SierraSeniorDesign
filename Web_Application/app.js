const express = require('express');
const fs = require('fs');
const path = require('path');

const app = express();
const PORT = 3000;

const ROOT_DIR = path.join(__dirname, 'file-system-root');

// Ensure root directory exists
if (!fs.existsSync(ROOT_DIR)) {
    fs.mkdirSync(ROOT_DIR);
}

app.use(express.static('public'));
app.use(express.json());

// Endpoint to get file structure
app.get('/files', (req, res) => {
    const getFileTree = (dirPath) => {
        const files = fs.readdirSync(dirPath);
        return files.map(file => {
            const filePath = path.join(dirPath, file);
            const isDirectory = fs.lstatSync(filePath).isDirectory();
            return {
                name: file,
                isDirectory,
                children: isDirectory ? getFileTree(filePath) : []
            };
        });
    };
    res.json(getFileTree(ROOT_DIR));
});

// Endpoint to create a new directory
app.post('/create-directory', (req, res) => {
    const { dirPath } = req.body;
    const fullPath = path.join(ROOT_DIR, dirPath);
    if (!fs.existsSync(fullPath)) {
        fs.mkdirSync(fullPath, { recursive: true });
    }
    res.sendStatus(200);
});

// Endpoint to create a new file
app.post('/create-file', (req, res) => {
    const { filePath } = req.body;
    const fullPath = path.join(ROOT_DIR, filePath);
    if (!fs.existsSync(fullPath)) {
        fs.writeFileSync(fullPath, '');
    }
    res.sendStatus(200);
});

// Endpoint to delete a file or directory
app.post('/delete', (req, res) => {
    const { path: deletePath } = req.body;
    const fullPath = path.join(ROOT_DIR, deletePath);
    if (fs.existsSync(fullPath)) {
        fs.rmSync(fullPath, { recursive: true, force: true });
    }
    res.sendStatus(200);
});

app.listen(PORT, () => {
    console.log(`Server is running on http://localhost:${PORT}`);
});
