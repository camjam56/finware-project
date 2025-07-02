import React, { createContext, useState } from 'react';

export const UserContext = createContext();

export const UserProvider = ({ children }) => {
  const [user, setUser] = useState(null);
  const [loggedInStatus, setLoggedInStatus] = useState(false);

  return (
    <UserContext.Provider
      value={{ user, setUser, loggedInStatus, setLoggedInStatus }}
    >
      {children}
    </UserContext.Provider>
  );
};
