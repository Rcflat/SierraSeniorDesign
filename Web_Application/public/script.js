document.addEventListener('DOMContentLoaded', function () {
    const fileSystemElement = document.getElementById('file-system');
    const logPanel = document.getElementById('log');

    function fetchFileSystem() {
        fetch('/files')
            .then(response => response.json())
            .then(data => {
                fileSystemElement.innerHTML = generateFileSystemHtml(data);
                addEventListeners();
            });
    }

    function generateFileSystemHtml(files) {
        let html = '<ul>';
        files.forEach(file => {
            if (file.isDirectory) {
                html += `<li><span class="directory">${file.name}</span>${generateFileSystemHtml(file.children)}</li>`;
            } else {
                html += `<li><span class="file">${file.name}</span></li>`;
            }
        });
        html += '</ul>';
        return html;
    }

    function addEventListeners() {
        const directories = document.querySelectorAll('.directory');
        directories.forEach(directory => {
            directory.addEventListener('click', function () {
                const sublist = this.nextElementSibling;
                if (sublist) {
                    sublist.style.display = sublist.style.display === 'none' ? 'block' : 'none';
                }
                logPanel.innerHTML += `<div>Clicked on directory: ${this.textContent}</div>`;
            });
        });

        const files = document.querySelectorAll('.file');
        files.forEach(file => {
            file.addEventListener('click', function () {
                logPanel.innerHTML += `<div>Clicked on file: ${this.textContent}</div>`;
            });
        });
    }

    fetchFileSystem();
});

