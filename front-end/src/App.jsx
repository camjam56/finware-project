import React from 'react';
import Login from './components/Login';
import Dashboard from './components/Dashboard';
import Register from './components/Register';
import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import { ROUTES } from './routes/routes.js';

function App() {
  return (
    <div>
      <Router>
        <Routes>
          <Route path={ROUTES.login} element={<Login />} />
          <Route path={ROUTES.dashboard} element={<Dashboard />} />
          <Route path={ROUTES.register} element={<Register />} />
        </Routes>
      </Router>
    </div>
  );
}

export default App;
