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
	const [activePanel, setActivePanel] = useState(null);
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

			<div className="content-container">
				<aside className="accounts-column">
					<h2 className="trade-history-title">Trade History</h2>
					<div className="trade-history-column">
						<div className="sort-by-container">
							<h3>Sort by:</h3>
							<select className="sort-by-dropdown">
								<option value="time">Most Recent</option>
								<option value="stock">Stock</option>
								<option value="type">Trade Type</option>
								<option value="quantity">Quantity</option>
							</select>
						</div>
					</div>
					<button
						className="open-trade-button"
						onClick={() => setActivePanel('openTrade')}
					>
						Open New Trade
					</button>

					<button className="signout-button" onClick={handleSignOut}>
						Sign Out
					</button>

				</aside>

				<div className="right-side-container">
					{activePanel === 'openTrade' && (
						<div className="open-trade-panel">
							<h2 className="open-trade-title">Opening Trade Details</h2>
						</div>
					)}

					{activePanel === 'tradeInformation' && (
						<div className="trade-information-panel">
						</div>
					)}
				</div>
			</div>
		</div >
	);
};

export default Dashboard;
