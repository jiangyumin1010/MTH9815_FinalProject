/**
 * executionservice.hpp
 * Defines the Service and Listener for executions.
 *
 * @author Breman Thuraisingham
 * @coauthor Yumin Jiang
 */
#ifndef EXECUTION_SERVICE_HPP
#define EXECUTION_SERVICE_HPP

#include <string>
#include "soa.hpp"
#include "execution.hpp"
#include "marketdataservice.hpp"


// Pre-declearations to avoid errors.
template<typename T>
class ExecutionServiceListener;

/**
 * Service for executing orders on an exchange.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class ExecutionService : public Service<string,ExecutionOrder <T> >
{
public:

    // Ctor
    ExecutionService();

    // Get data on our service given a key
    ExecutionOrder<T>& GetData(string _id);

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(ExecutionOrder<T>& _data);

    // Add a listener to the Service
    void AddListener(ServiceListener<ExecutionOrder<T>>* _listener);

    // Get all listeners on the Service
    const vector<ServiceListener<ExecutionOrder<T>>*>& GetListeners() const;

    // Get the listener of the service
    ExecutionServiceListener<T>* GetListener();

    // Execute order upon receiving an execution request.
    void ExecuteOrder(ExecutionOrder<T>& _executionOrder);

private:
    map<string, ExecutionOrder<T>> executionOrders;
    vector<ServiceListener<ExecutionOrder<T>>*> listeners;
    ExecutionServiceListener<T>* listener;
};

template<typename T>
ExecutionService<T>::ExecutionService()
{
    executionOrders = map<string, ExecutionOrder<T>>();
    listeners = vector<ServiceListener<ExecutionOrder<T>>*>();
    listener = new ExecutionServiceListener<T>(this);
}

template<typename T>
ExecutionOrder<T>& ExecutionService<T>::GetData(string _id)
{
    return executionOrders[_id];
}

template<typename T>
void ExecutionService<T>::OnMessage(ExecutionOrder<T>& _data)
{
    executionOrders[_data.GetProduct().GetProductId()] = _data;
}

template<typename T>
void ExecutionService<T>::AddListener(ServiceListener<ExecutionOrder<T>>* _listener)
{
    listeners.push_back(_listener);
}

template<typename T>
const vector<ServiceListener<ExecutionOrder<T>>*>& ExecutionService<T>::GetListeners() const
{
    return listeners;
}

template<typename T>
ExecutionServiceListener<T>* ExecutionService<T>::GetListener()
{
    return listener;
}

template<typename T>
void ExecutionService<T>::ExecuteOrder(ExecutionOrder<T>& _executionOrder)
{
    executionOrders[_executionOrder.GetProduct().GetProductId()] = _executionOrder;

    for (auto& l : listeners)
    {
        l->ProcessAdd(_executionOrder);
    }
}

/**
* Service Listener
* subscribe data from the BondAlgoExecutionService
* Type T is the product type.
*/
template<typename T>
class ExecutionServiceListener : public ServiceListener<AlgoExecution<T>>
{
public:
    // ctor
    ExecutionServiceListener(ExecutionService<T>* _service);

    // Listener callback to process an add event to the Service
    void ProcessAdd(AlgoExecution<T>& _data);

    // Listener callback to process a remove event to the Service
    void ProcessRemove(AlgoExecution<T>& _data);

    // Listener callback to process an update event to the Service
    void ProcessUpdate(AlgoExecution<T>& _data);

private:
    ExecutionService<T>* service;
};

template<typename T>
ExecutionServiceListener<T>::ExecutionServiceListener(ExecutionService<T>* _service)
{
    service = _service;
}

template<typename T>
void ExecutionServiceListener<T>::ProcessAdd(AlgoExecution<T>& _data)
{
    ExecutionOrder<T>* execution_order = _data.GetExecutionOrder();

    service->OnMessage(*execution_order);

    service->ExecuteOrder(*execution_order);
}

template<typename T>
void ExecutionServiceListener<T>::ProcessRemove(AlgoExecution<T>& _data) {}

template<typename T>
void ExecutionServiceListener<T>::ProcessUpdate(AlgoExecution<T>& _data) {}


#endif
