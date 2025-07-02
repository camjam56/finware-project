import React, { useState, useContext } from 'react';
import './Login.css';
import { useNavigate } from 'react-router-dom';
import { UserContext } from '../context/UserContext';
import { ROUTES } from '../routes/routes.js';

const Login = () => {
  const navigate = useNavigate();
  const [username, setUsername] = useState('');
  const [password, setPassword] = useState('');
  const { setUser, setLoggedInStatus } = useContext(UserContext);
  const [message, setMessage] = useState('');

  const handleLogin = async () => {
    try {
      const response = await fetch('http://localhost:5656', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          username,
          password,
        }),
      });

      const result = await response.text();

      if (result === '1') {
        //Login succeeded, result will be receiving login_success int value from backend
        setMessage(`Welcome, ${username}!`);
        setUser(username);
        setLoggedInStatus(true);
        navigate(ROUTES.dashboard);
      } else {
        //Login fails due to incorrect signin details
        setMessage('Invalid login credentials');
      }
    } catch (error) {
      console.error('Error logging in:', error);
      setMessage('Sorry, unexpected error occured, unable to connect');
    }
  };

  return (
    <div className="login-container">
      <h2>Welcome to</h2>
      <h1 className="main-title">FinWare Project</h1>
      <h3>Please Login</h3>
      <input
        type="text"
        className="input-field"
        placeholder="Username"
        value={username}
        onChange={(e) => setUsername(e.target.value)}
      />
      <br />
      <input
        type="password"
        className="input-field"
        placeholder="Password"
        value={password}
        onChange={(e) => setPassword(e.target.value)}
      />
      <br />
      <button onClick={handleLogin}>Login</button>
      <button onClick={() => navigate(ROUTES.register)}>Create Account</button>
      <p>{message}</p>
    </div>
  );
};

export default Login;
