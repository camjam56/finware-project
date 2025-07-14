import React, { useState, useContext } from 'react';
import './Dashboard.css';
import { useNavigate } from 'react-router-dom';
import { ROUTES } from '../routes/routes.js';
import { UserContext } from '../context/UserContext';


//async function openTrade(tradeData) {
//	const response = await fetch("http://localhost:5656/trade", {
//		method: "POST",
//		body: JSON.stringify(tradeData),
//		headers: { "Content-Type": "application/json" },
//	});
//
//	if (!response.ok) {
//		throw new Error("Failed to open trade");
//	}
//
//	return await response.json();
//}




const Dashboard = () => {
	const navigate = useNavigate();
	const { user, loggedInStatus } = useContext(UserContext);

	const handleSignOut = () => {
		navigate(ROUTES.login);
	};

	return (
		<div className="dashboard-container">
			<header className="dashboard-title">
				<h1>{user}'s Dashboard</h1>
			</header>

			<aside className="accounts-column">
				<h2>Your Account's</h2>
				<div className="trade-history-box">

				</div>
			</aside>

			<section>

			</section>

			<footer className="signout-footer">
				<button className="signout-button" onClick={handleSignOut}>
					Sign Out
				</button>
			</footer>
		</div>
	);
};

export default Dashboard;
