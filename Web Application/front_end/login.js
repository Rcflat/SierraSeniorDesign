document.getElementById('login-form').addEventListener('submit', function(event) {
    event.preventDefault(); // Prevent form submission

    const username = document.getElementById('username').value;
    const password = document.getElementById('password').value;

    fetch('http://localhost:3000/login', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({ username, password })
    })
    .then(response => response.json())
    .then(data => {
        if (data.message === 'Login successful!') {
            alert('Login successful!');
            // Redirect to another page or perform other actions on successful login
        } else {
            alert('Invalid username or password');
        }
    })
    .catch(error => console.error('Error:', error));
});
