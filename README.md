# RSI-Trading-Sim-bot

A simple stock trading bot that uses moving average crossover and RSI (Relative Strength Index) strategies to make buy/sell decisions. The bot simulates trading on historical price data, updates the market status regularly, and tracks the account's balance, trade history, and profit/loss.

## Features

- **Moving Average Crossover Strategy**: The bot uses short and long moving averages to determine when to buy or sell a stock.
- **RSI Strategy**: The bot uses the Relative Strength Index (RSI) to identify overbought or oversold conditions, and make buy or sell decisions based on thresholds.
- **Market Status Display**: Regular updates on current market price, RSI, and account status including balance, shares held, and profit/loss.
- **Trade Execution**: The bot simulates trade execution (buy/sell) based on calculated signals, updating the account's balance and trade history.
- **Real-Time Data Simulation**: The bot simulates real-time price changes using a normal distribution for random price fluctuations.

## Requirements

- C++11 or later
- Windows OS (for `Sleep()` function)

### Output example
=== Trading Bot Configuration ===
Recommended values:
- Short MA Window: 5
- Long MA Window: 10
- RSI Period: 14
- RSI Buy Threshold: 30
- RSI Sell Threshold: 70
- Update Interval: 3

Enter starting balance ($1000-$1000000): 10000.0

=== Trading Parameters Setup ===
Enter short moving average window (2-10): 5
Enter long moving average window (6-20): 10
Enter RSI period (2-30): 14
Enter RSI buy threshold (0-40): 30
Enter RSI sell threshold (60-100): 70
Enter update interval in seconds (1-60): 3

Initializing with sample data...

Starting trading bot with your parameters...
Press Ctrl+C to stop the bot

=== Market Status ===
Current Price: $110.00
RSI: 45.00

=== Account Status ===
Cash Balance: $10000.00
Shares Held: 0
Current Share Value: $0.00
Total Value: $10000.00
Profit/Loss: $0.00 (0.00%)

Recent Prices: $100.00 $105.00 $110.00 $107.00 $113.00

Moving Average Signal: hold
RSI Signal: buy

Waiting 3 seconds for next update...

![Config1](https://github.com/user-attachments/assets/9a20c3a4-55d3-4976-b811-03826212ca1f)

![Config2](https://github.com/user-attachments/assets/13abe3ae-061b-4c9e-ada7-8515023dce9d)


