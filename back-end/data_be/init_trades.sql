CREATE TABLE trades (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	username TEXT NOT NULL,
	symbol TEXT NOT NULL,
	stock_name TEXT NOT NULL,
	quantity INTEGER NOT NULL,
	price_per_share REAL NOT NULL,
	total_cost REAL NOT NULL,
	trade_type TEXT CHECK(trade_type IN ('BUY', 'SELL')) NOT NULL,
	trade_time TEXT NOT NULL
);
