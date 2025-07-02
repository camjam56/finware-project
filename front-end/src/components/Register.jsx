import React, { useState, useContext } from 'react';
import './Register.css';
import { useNavigate } from 'react-router-dom';
import { UserContext } from '../context/UserContext';
import { ROUTES } from '../routes/routes.js';

const handleRegister = () => {
  const navigate = useNavigate();
  const { user, loggedInStatus } = useContext(UserContext);

  return (
    <div className="register-container">
      <h1 className="main-title">Register</h1>
      <h2>Please create an account to begin trading with Finware-Project</h2>
    </div>
  );
};

export default handleRegister;
