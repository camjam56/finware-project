import React, { useEffect, useState, useContext, useMemo } from 'react';
import './Dashboard.css';
import { useNavigate } from 'react-router-dom';
import { ROUTES } from '../routes/routes.js';
import { UserContext } from '../context/UserContext';

const Dashboard = () => {
	const [stockSymbol, setStockSymbol] = useState('');
	const [tradeType, setTradeType] = useState('');
	const [quantity, setQuantity] = useState('');
	const [pricePerShare, setPricePerShare] = useState(null);
	const [userTrades, setUserTrades] = useState([]);
	const [sortBy, setSortBy] = useState('time');
	const navigate = useNavigate();
	const { user } = useContext(UserContext);

	const handleSignOut = () => {
		navigate(ROUTES.login);
	};

	const sortedTrades = useMemo(() => {
		if (!userTrades) return [];

		const tradesCopy = [...userTrades];

		switch (sortBy) {
			case 'stock':
				return tradesCopy.sort((a, b) =>
					a.symbol.localeCompare(b.symbol));

			case 'type':
				return tradesCopy.sort((a, b) =>
					a.tradeType.localeCompare(b.tradeType));

			case 'quantity':
				return tradesCopy.sort((a, b) => b.quantity - a.quantity);

			case 'time':
			default:
				return tradesCopy.sort(
					(a, b) => new Date(b.trade_time) - new Date(a.trade_time));
		}
	}, [userTrades, sortBy]);


	const mockStocks = [
		{ symbol: 'GPU', name: 'Graphics Cards Inc.', price: 2312.12 },
		{ symbol: 'MBRD', name: 'Motherboards Inc.', price: 165.56 },
		{ symbol: 'SSD', name: 'Solid-State Drives Inc.', price: 44.24 },
	];

	useEffect(() => {
		const fetchUserTrades = async () => {
			try {
				const response = await fetch(`http://localhost:5656/trades?user=${user}`);
				if (!response.ok) throw new Error("Failed to fetch the user's trade history");

				const data = await response.json();
				setUserTrades(data.trades);

			} catch (error) {
				console.error("Error fetching user's trade history");
			}
		};

		if (user) fetchUserTrades();
	}, [user]);

	useEffect(() => {
		const selected = mockStocks.find(stock => stock.symbol === stockSymbol);
		setPricePerShare(selected ? selected.price : null);
	}, [stockSymbol]);


	const handleSubmit = async (e) => {

		e.preventDefault();

		const tradeData = {
			user,
			stockSymbol,
			tradeType,
			quantity: Number(quantity),
			pricePerShare: Number(pricePerShare),
		};

		try {
			const response = await fetch("http://localhost:5656/trade", {
				method: "POST",
				headers: { "Content-Type": "application/json" },
				body: JSON.stringify(tradeData),
			});

			if (!response.ok) throw new Error("Failed with trade submission");

			const data = await response.json();
			console.log("Trade submitted:", data);

			setStockSymbol('');
			setTradeType('');
			setQuantity('');
			setPricePerShare(null);


			const tradeRes = await fetch(`http://localhost:5656/trades?user=${user}`);
			if (!tradeRes.ok) throw new Error("Failed to refresh user's trades");
			const newTrades = await tradeRes.json();
			setUserTrades(newTrades.trades);


		} catch (error) {
			console.error("Error with trade submission:", error);
		}
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
							<select className="sort-by-dropdown"
								value={sortBy}
								onChange={(e) => setSortBy(e.target.value)}>
								<option value="time">Most Recent</option>
								<option value="stock">Stock</option>
								<option value="type">Trade Type</option>
								<option value="quantity">Quantity</option>
							</select>
						</div>

						<div className="trade-history-list">

							{sortedTrades.map((trade, index) => (
								<div key={index} className="trade-entry">
									<p>
										<strong>{trade.tradeType}</strong> {trade.quantity} x {trade.symbol} @ ${trade.pricePerShare.toFixed(2)}
									</p>
									<p>
										Total: ${(trade.pricePerShare * trade.quantity).toFixed(2)} — {new Date(trade.tradeTime).toLocaleString()}
									</p>
								</div>
							))}


						</div>
					</div>
					<button onClick={handleSignOut} className="signout-button">
						Sign Out
					</button>
				</aside>

				<div className="right-side-container">
					<div className="open-trade-panel">
						<h2 className="trade-details-title">New Trade</h2>
						<form onSubmit={handleSubmit} className="trade-form">
							<label className="trade-label">
								Stock Symbol:
								<select
									value={stockSymbol}
									onChange={(e) => setStockSymbol(e.target.value)}
									required
								>
									<option value="">-- Select Stock --</option>
									{mockStocks.map((stock) => (
										<option key={stock.symbol} value={stock.symbol}>
											{stock.symbol} - {stock.name}
										</option>
									))}
								</select>
							</label>

							<label className="trade-label">
								Trade Type:
								<select
									value={tradeType}
									onChange={(e) => setTradeType(e.target.value)}
								>
									<option value="">-- Select Type --</option>
									<option value="BUY">BUY</option>
									<option value="SELL">SELL</option>
								</select>
							</label>

							<label className="trade-label">
								Quantity:
								<input
									type="number"
									value={quantity}
									required
									min="1"
									onChange={(e) => setQuantity(e.target.value)}
								/>
							</label>

							<label className="trade-label">
								Price Per Share:{" "}
								<span>
									{pricePerShare !== null ? `$${pricePerShare.toFixed(2)}` : '--'}
								</span>
							</label>

							<label className="trade-label">
								Total Cost:{" "}
								<span>
									{pricePerShare !== null ? `$${pricePerShare.toFixed(2) * quantity}` : '--'}
								</span>
							</label>

							<button className="submit-trade-button" type="submit">
								Submit Trade
							</button>
						</form>
					</div>
				</div>
			</div >
		</div >
	);
};

export default Dashboard;
