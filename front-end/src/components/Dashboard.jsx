import React, { useState, useContext } from 'react';
import './Dashboard.css';
import { useNavigate } from 'react-router-dom';
import { ROUTES } from '../routes/routes.js';
import { UserContext } from '../context/UserContext';

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
        <p>Account 1 Placeholder</p>
        <p>Account 2 Placeholder</p>
        <p>Account 3 Placeholder</p>
        <p>Account 4 Placeholder</p>
      </aside>

      <footer className="signout-footer">
        <button className="signout-button" onClick={handleSignOut}>
          Sign Out
        </button>
      </footer>
    </div>
  );
};

export default Dashboard;
