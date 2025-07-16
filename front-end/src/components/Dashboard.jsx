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
  const [stockSymbol, setStockSymbol] = useState('');
  const [tradeType, setTradeType] = useState('');
  const [quantity, setQuantity] = useState('');
  const [tradeTimestamp, setTradeTimestamp] = useState('');
  const navigate = useNavigate();
  const { user } = useContext(UserContext);

  const handleSignOut = () => {
    navigate(ROUTES.login);
  };

  const handleSubmitTrade = async (e) => {
    e.preventDefault();

    const tradeData = {
      stockSymbol,
      quantity: Number(quantity),
      tradeType,
    };

    try {
      const response = await fetch('http://localhost/5656/trade', {
        method: 'POST',
        header: { 'Content-Type': 'application/json' },
        body: JSON.stringify(tradeData),
      });

      if (!response.ok) {
        throw new Error('Error submitting trade');
      }

      const data = await response.json();
      console.log('Trade successfully submit', data);
    } catch (error) {
      console.error('Error making trade:', error);
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
              <select className="sort-by-dropdown">
                <option value="time">Most Recent</option>
                <option value="stock">Stock</option>
                <option value="type">Trade Type</option>
                <option value="quantity">Quantity</option>
              </select>
            </div>
          </div>
          <button className="signout-button" onClick={handleSignOut}>
            Sign Out
          </button>
        </aside>

        <div className="right-side-container">
          <div className="open-trade-panel">
            <h2 className="trade-details-title">New Trade</h2>
            <form className="trade-form">
              <label className="trade-label">
                Stock Symbol:
                <select
                  value={stockSymbol}
                  onChange={(e) => setStockSymbol(e.target.value)}
                  required
                >
                  <option value="">-- Select Stock --</option>
                  <option value="GPU">GPU</option>
                  <option value="MBRD">MBRD</option>
                  <option value="SSD">SSD</option>
                </select>
              </label>

              <label className="trade-label">
                Trade Type:
                <select
                  value={tradeType}
                  onChange={(e) => setTradeType(e.target.value)}
                >
                  <option value="buy">Buy</option>
                  <option value="sell">Sell</option>
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
                Price per Share:
                <input type="number" name="price" required step="0.01" />
              </label>

              <button className="submit-trade-button" type="submit">
                Submit Trade
              </button>
            </form>
          </div>
        </div>
      </div>
    </div>
  );
};

export default Dashboard;
