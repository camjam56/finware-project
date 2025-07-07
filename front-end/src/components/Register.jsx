import React, { useState, useContext } from 'react';
import './Register.css';
import { useNavigate } from 'react-router-dom';
import { UserContext } from '../context/UserContext';
import { ROUTES } from '../routes/routes.js';

const Register = () => {
	const navigate = useNavigate();
	const [regUsername, setRegUsername] = useState('');
	const [regPassword, setRegPassword] = useState('');
	const [passwordConfirm, setPasswordConfirm] = useState('');
	const [message, setMessage] = useState('');

	const handleRegister = () => {
		const passwordRegex = /^[A-Za-z0-9?!]+$/;

		if (!regPassword.match(passwordRegex)) {
			setMessage(`Invalid Password: May only contain letters, numbers, "?" and "!"`);
			return;
		}
		if (regPassword.trim() == '') {
			setMessage(`Invalid Password: Empty password`);
			return;
		}
		if (regPassword != passwordConfirm) {
			setMessage(`Passwords do not match`);
			return;
		}
		setMessage(`Success!`);
	}

	return (
		<div className="register-container">
			<h1 className="main-title">Register</h1>
			<h2>Please create an account to begin trading with Finware-Project</h2>
			<input
				type="text"
				className="input-field"
				placeholder="Username"
				value={regUsername}
				onChange={(e) => setRegUsername(e.target.value)}
			/>
			<input
				type="password"
				className="input-field"
				placeholder="Password"
				value={regPassword}
				onChange={(e) => setRegPassword(e.target.value)}
			/>
			<input
				type="password"
				className="input-field"
				placeholder="Confirm Password"
				value={passwordConfirm}
				onChange={(e) => setPasswordConfirm(e.target.value)}
			/>
			<button className="register-button" onClick={handleRegister}>Create Account</button>
			<div className="return-button">
				<h2>Click below to return to login page</h2>
				<button onClick={() => navigate(ROUTES.login)}>Return to Login</button>
			</div>
			<p>{message}</p>
		</div>
	);
};
export default Register;
