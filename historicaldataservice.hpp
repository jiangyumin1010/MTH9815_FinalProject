/**
 * historicaldataservice.hpp
 *
 * Defines the HistoricalDataService template class for historical data processing and persistence.
 * This service is used to process and persist historical data keyed on a persistent key.
 *
 * @author Breman Thuraisingham
 * @coauthor Yumin Jiang
 */

#ifndef HISTORICAL_DATA_SERVICE_HPP
#define HISTORICAL_DATA_SERVICE_HPP

#include "executionservice.hpp"
#include "inquiryservice.hpp"
#include "positionservice.hpp"
#include "riskservice.hpp"
#include "soa.hpp"
#include "streamingservice.hpp"
#include "utility.hpp"

// Forward declarations to avoid errors.
template <typename V> class HistoricalDataConnector;
template <typename V> class HistoricalDataListener;

/**
 * Service for processing and persisting historical data to a persistent store.
 * Keyed on some persistent key.
 * Type V is the data type to persist.
 */
template <typename V> class HistoricalDataService : Service<string, V> {

  public:
    // Constructors
    HistoricalDataService();
    HistoricalDataService(string _type);

    // Get data associated with a key
    V& GetData(string _key);

    // Callback for a Connector to invoke with new or updated data
    void OnMessage(V& _data);

    // Add a listener to the Service
    void AddListener(ServiceListener<V>* _listener);

    // Get all listeners on the Service
    const vector<ServiceListener<V>*>& GetListeners() const;

    // Get the connector of the service
    HistoricalDataConnector<V>* GetConnector();

    // Get the listener of the service
    ServiceListener<V>* GetServiceListener();

    // Get the service type
    // Possible values: "Position", "Risk", "Execution", "Streaming", "Inquiry"
    string GetServiceType() const;

    // Persist data locally
    void PersistData(string _persistKey, V& _data);

  private:
    map<string, V> historicalDatas;
    vector<ServiceListener<V>*> listeners;
    HistoricalDataConnector<V>* connector;
    ServiceListener<V>* listener;
    string type;
};

template <typename V> HistoricalDataService<V>::HistoricalDataService() {
    historicalDatas = map<string, V>();
    listeners = vector<ServiceListener<V>*>();
    connector = new HistoricalDataConnector<V>(this);
    listener = new HistoricalDataListener<V>(this);
    type = "Position";
}

template <typename V>
HistoricalDataService<V>::HistoricalDataService(string _type) {
    historicalDatas = map<string, V>();
    listeners = vector<ServiceListener<V>*>();
    connector = new HistoricalDataConnector<V>(this);
    listener = new HistoricalDataListener<V>(this);
    type = _type;
}

template <typename V> V& HistoricalDataService<V>::GetData(string _key) {
    return historicalDatas[_key];
}

template <typename V> void HistoricalDataService<V>::OnMessage(V& _data) {
    historicalDatas[_data.GetProduct().GetProductId()] = _data;
}

template <typename V>
void HistoricalDataService<V>::AddListener(ServiceListener<V>* _listener) {
    listeners.push_back(_listener);
}

template <typename V>
const vector<ServiceListener<V>*>&
HistoricalDataService<V>::GetListeners() const {
    return listeners;
}

template <typename V>
HistoricalDataConnector<V>* HistoricalDataService<V>::GetConnector() {
    return connector;
}

template <typename V>
ServiceListener<V>* HistoricalDataService<V>::GetServiceListener() {
    return listener;
}

template <typename V> string HistoricalDataService<V>::GetServiceType() const {
    return type;
}

template <typename V>
void HistoricalDataService<V>::PersistData(string _persistKey, V& _data) {
    connector->Publish(_data);
}

/**
 * Connector for Historical Data Service.
 * Type V is the data type to persist.
 * Core function: publish
 */
template <typename V> class HistoricalDataConnector : public Connector<V> {
  public:
    // Constructor
    HistoricalDataConnector(HistoricalDataService<V>* _service);

    // Publish data to the Connector
    void Publish(V& _data);

    // Subscribe data from the Connector
    void Subscribe(ifstream& _data);

  private:
    HistoricalDataService<V>* service;
};

template <typename V>
HistoricalDataConnector<V>::HistoricalDataConnector(
    HistoricalDataService<V>* _service) {
    service = _service;
}

template <typename V> void HistoricalDataConnector<V>::Publish(V& _data) {
    string _type = service->GetServiceType();
    ofstream _file;
    if (_type == "Position") {
        _file.open("Data/Output/positions.txt", ios::app);
        if (!_file.is_open()) {
            cerr << "Error: Unable to open file at "
                 << "Data/Output/positions.txt" << endl;
            return;
        }
    }
    if (_type == "Risk") {
        _file.open("Data/Output/risk.txt", ios::app);
        if (!_file.is_open()) {
            cerr << "Error: Unable to open file at "
                 << "Data/Output/risk.txt" << endl;
            return;
        }
    }
    if (_type == "Execution") {
        _file.open("Data/Output/executions.txt", ios::app);
        if (!_file.is_open()) {
            cerr << "Error: Unable to open file at "
                 << "Data/Output/execution.txt" << endl;
            return;
        }
    }
    if (_type == "Streaming") {
        _file.open("Data/Output/streaming.txt", ios::app);
        if (!_file.is_open()) {
            cerr << "Error: Unable to open file at "
                 << "Data/Output/streaming.txt" << endl;
            return;
        }
    }
    if (_type == "Inquiry") {
        _file.open("Data/Output/allinquiries.txt", ios::app);
        if (!_file.is_open()) {
            cerr << "Error: Unable to open file at "
                 << "Data/Output/allinquiries.txt" << endl;
            return;
        }
    }
    auto now = microsec_clock::local_time();
    _file << now << ",";

    vector<string> _dataStrings = _data.PrintFunction();
    for (auto& s : _dataStrings) {
        _file << s << ",";
    }
    _file << endl;
}

template <typename V>
void HistoricalDataConnector<V>::Subscribe(ifstream& _data) {}

/**
 * Service Listener subscribing data to Historical Data.
 * from BondPositionService, BondRiskService, BondExecutionService,
 * BondStreamingService, and BondInquiryService Type V is the data type to
 * persist.
 */
template <typename V> class HistoricalDataListener : public ServiceListener<V> {

  private:
    HistoricalDataService<V>* service;

  public:
    // Constructor and Destructor
    HistoricalDataListener(HistoricalDataService<V>* _service);
    ~HistoricalDataListener();

    // Listener callback to process an add event to the Service
    void ProcessAdd(V& _data);

    // Listener callback to process a remove event to the Service
    void ProcessRemove(V& _data);

    // Listener callback to process an update event to the Service
    void ProcessUpdate(V& _data);
};

template <typename V>
HistoricalDataListener<V>::HistoricalDataListener(
    HistoricalDataService<V>* _service) {
    service = _service;
}

template <typename V> HistoricalDataListener<V>::~HistoricalDataListener() {}

template <typename V> void HistoricalDataListener<V>::ProcessAdd(V& _data) {
    string _persistKey = _data.GetProduct().GetProductId();
    service->PersistData(_persistKey, _data);
}

template <typename V> void HistoricalDataListener<V>::ProcessRemove(V& _data) {}

template <typename V> void HistoricalDataListener<V>::ProcessUpdate(V& _data) {}

#endif
