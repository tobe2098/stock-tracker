# stocktracker
A stock-tracker using Qt and C++


## To-do
<!-- - [ ] If the stock is not found, search the database for it and add it to tracked stocks. For now that cannot happen because we load all on startup. -->


- Cross button for stocks (deletes from memory but not from database?) DONE
- Delete button for stocks (deletes from memory and from database) DONE
- Improve X axis manipulation. Maybe a time scroller? Done

- Fetching notification only during fetching, not require click. Doing (also notification when deleted, only appear in  the bottom right)
- Rate limit calculations, tracking and message to avoid, but override. Doing Partially done, only thing left is what we do with exceeded rate limits.
- Separate thread for fetching each (reason for queues apart from rate limits). Doing
- Waiting timer for historical request in bottom left, while the other progress bars are in the right? If possible

- Load json from API query for history (try to make it a rich query, intra and interday) DONE
- Review annotations for what can be done (findstock and update without deleting, sorted stocks)
- Graph button for
- Ability to track your portfolio (introduce manually, stored).
- Display API requests left. Recovered either from DB or by checking each stored stock (problem is if deleted). Store a queue of timestamps
- Special button for the API request.
- Change chart to be candles.
- Load historic data of stocks on startup
- Retrieving data from API request progress bar?
- Proper company name?
- No legend
- Brighter title
- For eachh stock if timestamp is not zero recall historical data
- Load vs download data buttons
- Threads to load all existing stock data, available in the graph page
- Settings or option to switch from INTRADAY to DAILY. Maybe DAILY is better, or INTRADAY? interval between datapoints settings, Alpha Vantage. Plotting and others does not change bc we were always storing the timestamp.
- Qsettings with theme as well?
- Storing settings and usage queue of AlphaVantage key

- Graph buttons to set the zoom at past 1D,1M,3M,6M,1Y,2Y,5Y,All

- Dragging items in the list, changing order and sorting options
- Delete "Select a stock..." When at least one exists.