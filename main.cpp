#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <ctime>
#include <windows.h>
#include <cmath>
#include <limits>
#include <random>
#include <iomanip>

using namespace std;

// Structure for holding price data
struct DataPoint {
    time_t timestamp;
    double close;
};

// Structure for holding account information
struct AccountStatus {
    double balance;
    double initialBalance;
    int shares;
    vector<pair<string, double>> tradeHistory;

    AccountStatus(double startingBalance) : 
        balance(startingBalance), 
        initialBalance(startingBalance), 
        shares(0) {}

    double getProfitLoss() const {
        if (tradeHistory.empty()) return 0.0;
        return balance - initialBalance + (shares * tradeHistory.back().second);
    }

    double getProfitLossPercentage() const {
        if (tradeHistory.empty()) return 0.0;
        double totalValue = balance + (shares * tradeHistory.back().second);
        return ((totalValue - initialBalance) / initialBalance) * 100.0;
    }
};

// Structure for holding trading parameters
struct TradingParams {
    int shortWindow;
    int longWindow;
    int rsiPeriod;
    double rsiBuyThreshold;
    double rsiSellThreshold;
    int sleepDuration;  // in seconds
};

// Function to get user input with validation
template<typename T>
T getValidInput(const string& prompt, T min_val, T max_val) {
    T value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= min_val && value <= max_val) {
            break;
        }
        cout << "Invalid input. Please enter a value between " << min_val << " and " << max_val << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return value;
}

// Function to get trading parameters from user
TradingParams getTradingParams() {
    TradingParams params;
    cout << "\n=== Trading Parameters Setup ===\n";
    
    params.shortWindow = getValidInput<int>("Enter short moving average window (2-10): ", 2, 10);
    params.longWindow = getValidInput<int>("Enter long moving average window (" + to_string(params.shortWindow + 1) + "-20): ", 
                                         params.shortWindow + 1, 20);
    
    params.rsiPeriod = getValidInput<int>("Enter RSI period (2-30): ", 2, 30);
    params.rsiBuyThreshold = getValidInput<double>("Enter RSI buy threshold (0-40): ", 0.0, 40.0);
    params.rsiSellThreshold = getValidInput<double>("Enter RSI sell threshold (60-100): ", 60.0, 100.0);
    params.sleepDuration = getValidInput<int>("Enter update interval in seconds (1-60): ", 1, 60);
    
    return params;
}

// Fetch historical price data (mock data for now)
vector<DataPoint> fetchData() {
    vector<DataPoint> data = {
        {1, 100}, {2, 105}, {3, 110}, {4, 107},
        {5, 113}, {6, 120}, {7, 125}, {8, 130},
        {9, 128}, {10, 135}, {11, 140}, {12, 145}
    };
    return data;
}

// Calculate Moving Average
vector<double> movingAverage(const vector<DataPoint>& data, int window) {
    vector<double> ma;
    for (size_t i = window - 1; i < data.size(); i++) {
        double sum = 0;
        for (size_t j = i - window + 1; j <= i; j++) {
            sum += data[j].close;
        }
        ma.push_back(sum / window);
    }
    return ma;
}

// Calculate RSI (Relative Strength Index)
double calculateRSI(const vector<DataPoint>& data, int period = 14) {
    if (data.size() < period + 1) {
        return 50.0;  // Return neutral RSI if not enough data
    }

    vector<double> gains, losses;
    // Calculate price changes and separate into gains and losses
    for (size_t i = 1; i < data.size(); i++) {
        double change = data[i].close - data[i-1].close;
        gains.push_back(change > 0 ? change : 0.0);
        losses.push_back(change < 0 ? -change : 0.0);
    }

    if (gains.size() < period) {
        return 50.0;
    }

    // Calculate initial averages
    double avgGain = 0.0;
    double avgLoss = 0.0;
    
    size_t startIdx = gains.size() - period;
    for (size_t i = startIdx; i < gains.size(); i++) {
        avgGain += gains[i];
        avgLoss += losses[i];
    }
    
    avgGain /= period;
    avgLoss /= period;

    if (avgLoss < 0.000001) {  
        return 100.0;
    }

    double rs = avgGain / avgLoss;
    double rsi = 100.0 - (100.0 / (1.0 + rs));
    
    return max(0.0, min(100.0, rsi));
}

// Moving Average Crossover strategy
string movingAverageCrossover(const vector<DataPoint>& data, const TradingParams& params) {
    vector<double> shortMA = movingAverage(data, params.shortWindow);
    vector<double> longMA = movingAverage(data, params.longWindow);

    if (shortMA.back() > longMA.back()) {
        return "buy";
    } else if (shortMA.back() < longMA.back()) {
        return "sell";
    } else {
        return "hold";
    }
}

// RSI-based strategy
string rsiStrategy(const vector<DataPoint>& data, const TradingParams& params) {
    double rsi = calculateRSI(data, params.rsiPeriod);

    if (rsi < params.rsiBuyThreshold) {
        return "buy";
    } else if (rsi > params.rsiSellThreshold) {
        return "sell";
    } else {
        return "hold";
    }
}

// Execute the trade with account tracking
void executeTrade(const string& action, double price, AccountStatus& account) {
    int amount = rand() % 5 + 1; 
    double totalCost = amount * price;
    
    if (action == "buy") {
        if (account.balance >= totalCost) {
            account.balance -= totalCost;
            account.shares += amount;
            cout << "Buying " << amount << " units at $" << fixed << setprecision(2) << price << " USD" << endl;
            account.tradeHistory.push_back({action, price});
        } else {
            cout << "Insufficient funds to buy " << amount << " units" << endl;
        }
    } else if (action == "sell") {
        if (account.shares >= amount) {
            account.balance += totalCost;
            account.shares -= amount;
            cout << "Selling " << amount << " units at $" << fixed << setprecision(2) << price << " USD" << endl;
            account.tradeHistory.push_back({action, price});
        } else {
            cout << "Insufficient shares to sell " << amount << " units" << endl;
        }
    } else {
        cout << "No action taken (Hold)" << endl;
    }
}

// Function to simulate price movement
double generateNextPrice(double currentPrice) {
    static random_device rd;
    static mt19937 gen(rd());
    static normal_distribution<> d(0, 0.05); // Increased volatility to 5% standard deviation
    
    double percentageChange = d(gen);
    return currentPrice * (1 + percentageChange);
}

// Update price data with new values
void updateData(vector<DataPoint>& data) {
    double lastPrice = data.back().close;
    double newPrice = generateNextPrice(lastPrice);
    
    for (size_t i = 0; i < data.size() - 1; i++) {
        data[i] = data[i + 1];
    }
 
    data.back() = {time(nullptr), newPrice};
}

// Display market status with account information
void displayMarketStatus(const vector<DataPoint>& data, double rsi, const AccountStatus& account) {
    system("cls");
    cout << "=== Market Status ===\n";
    cout << "Current Price: $" << fixed << setprecision(2) << data.back().close << endl;
    cout << "RSI: " << fixed << setprecision(2) << rsi << endl;
    
    cout << "\n=== Account Status ===\n";
    cout << "Cash Balance: $" << fixed << setprecision(2) << account.balance << endl;
    cout << "Shares Held: " << account.shares << endl;
    cout << "Current Share Value: $" << fixed << setprecision(2) << (account.shares * data.back().close) << endl;
    cout << "Total Value: $" << fixed << setprecision(2) << (account.balance + account.shares * data.back().close) << endl;
    
    double profitLoss = account.balance - account.initialBalance + (account.shares * data.back().close);
    double profitLossPercent = (profitLoss / account.initialBalance) * 100.0;
    cout << "Profit/Loss: $" << fixed << setprecision(2) << profitLoss << " (" 
         << fixed << setprecision(2) << profitLossPercent << "%)" << endl;

    cout << "\nRecent Prices: ";
    for (size_t i = max(0, (int)data.size() - 5); i < data.size(); i++) {
        cout << "$" << fixed << setprecision(2) << data[i].close << " ";
    }
    cout << "\n\n";
}

int main() {
    cout << "=== Trading Bot Configuration ===\n";
    cout << "Recommended values:\n";
    cout << "- Short MA Window: 5\n";
    cout << "- Long MA Window: 10\n";
    cout << "- RSI Period: 14\n";
    cout << "- RSI Buy Threshold: 30\n";
    cout << "- RSI Sell Threshold: 70\n";
    cout << "- Update Interval: 3\n\n";
    
    // Get initial account balance
    double initialBalance = getValidInput<double>("Enter starting balance ($1000-$1000000): ", 1000.0, 1000000.0);
    AccountStatus account(initialBalance);
    
    TradingParams params = getTradingParams();
    
    vector<DataPoint> data = fetchData();
    cout << "\nInitializing with sample data...\n";
    Sleep(2000);

    cout << "\nStarting trading bot with your parameters...\n";
    cout << "Press Ctrl+C to stop the bot\n\n";
    Sleep(2000);

    srand(time(nullptr));

    while (true) {
        try {
            updateData(data);
            double currentRsi = calculateRSI(data, params.rsiPeriod);
            
            displayMarketStatus(data, currentRsi, account);
            
            string actionMA = movingAverageCrossover(data, params);
            cout << "Moving Average Signal: " << actionMA << endl;
            executeTrade(actionMA, data.back().close, account);

            string actionRSI = rsiStrategy(data, params);
            cout << "RSI Signal: " << actionRSI << endl;
            executeTrade(actionRSI, data.back().close, account);

            cout << "\nWaiting " << params.sleepDuration << " seconds for next update...\n";
            Sleep(params.sleepDuration * 1000);
        } catch (const exception& e) {
            cout << "Error occurred: " << e.what() << endl;
            Sleep(2000);
        }
    }

    return 0;
}
