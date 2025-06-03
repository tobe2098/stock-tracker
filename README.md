# stocktracker
A stock-tracker using Qt and C++


## To-do
- [ ] If the stock is not found, search the database for it and add it to tracked stocks. For now that cannot happen because we load all on startup.


- Cross button for stocks (deletes from memory but not from database?)
- Delete button for stocks (deletes from memory and from database)

- Load json from API query for history (try to make it a rich query, intra and interday)
- Review annotations for what can be done (findstock and update without deleting)
- Fetching notiffication only during fetching, not require click.
- Ability to track your portfolio (introduce manually, stored).
- Display API requests left. Recovered either from DB or by checking each stored stock (problem is if deleted). Store a queue of timestamps
- Special button for the API request.
- Change chart to be candles.
- Improve X axis manipulation. Maybe a time scroller?