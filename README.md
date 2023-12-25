## Description
This is the final project for MFE course MTH_9815.

**Trading System**
Develop a bond trading system for US Treasuries with seven securities: 2Y, 3Y, 5Y, 7Y, 10Y, 20Y, and 30Y. Look up the CUSIPS, coupons, and maturity dates for each security. Ticker is T.
We have a new definition of a Service in soa.hpp, with the concept of a ServiceListener and Connector also defined. A cServiceListener is a listener to events on the service where data is added to the service, updated on the service, or removed from the service. A Connector is a class that flows data into the Service from some connectivity source (e.g. a socket, file, etc) via the Service.OnMessage() method. The Publish() method on the Connector publishes data to the connectivity source and can be invoked from a Service. Some Connectors are publish-only that do not invoke Service.OnMessage(). Some Connectors are subscribe-only where Publish() does nothing. Other Connectors can do both publish and subscribe.

Since github cannot upload too large data, I set the datasize generated as 10000 and this can be reset to 1000000 in DataGenerator.hpp by changing DATASIZE to 1000000.

## Build and Run the Project
1. Make sure Boost libraries installed.
2. Run `cmake .` in the project directory.
3. Run `make` to build the project.
4. Execute `./trade` 