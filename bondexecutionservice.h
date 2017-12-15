/**bondexecutionservice.hpp
 * Defines the data types and Service for bond executions
 * @author: Sijia Zhang
 */

#ifndef BOND_EXECUTION_SERVICE_HPP
#define BOND_EXECUTION_SERVICE_HPP

#include "executionservice.h"
#include "algoexecutionservice.h"

/**
 * Service for executing bond orders on an exchange.
 * Keyed on product identifier.
 * We use the type Bond instead of using template T
 */
class BondExecutionService : public ExecutionService<Bond>{

private:

    //define listener
    std::vector<ServiceListener<ExecutionOrder<Bond>>*> listeners;

    //define a map to find data on the service
    std::map<std::string, ExecutionOrder<Bond>> execution_data;

    //ctor
    BondExecutionService(){};

public:

    // Generate instance
    static BondExecutionService* Generate_Instance(){
        //we define this function to generate instance for class BondExecutionService.
        static BondExecutionService ins;
        return &ins;
    }

    //  pure virtual member functions in class Service.
    // Get data on our service given a key
    ExecutionOrder<Bond>& GetData(std::string key)  ;

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(ExecutionOrder<Bond> &data)  ;

    // Add a listener to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<ExecutionOrder<Bond>> *listener)  ;

    // Get all listeners on the Service.
    const std::vector< ServiceListener<ExecutionOrder<Bond>>* >& GetListeners() const  ;

    // Add Algo Execution to the service
    void AddAlgoExecution(AlgoExecution& ob);

    // Execute an order on a market
    void ExecuteOrder(const ExecutionOrder<Bond>& order, Market market);
};



/**
 * BondExecutionServiceListener passes the data from execution service
 * We use the type Bond instead of using template T.
 */

class BondExecutionServiceListener : public ServiceListener<AlgoExecution>{

private:

    //define a pointer of AlgoExecutionService
    BondExecutionService * bond_exe_service;

    //ctor
    BondExecutionServiceListener(){
        bond_exe_service=BondExecutionService::Generate_Instance();
    }

public:

    // Generate instance
    static BondExecutionServiceListener* Generate_Instance(){
        //we define this function to generate instance for class BondExecutionServiceListener.
        static BondExecutionServiceListener ins;
        return &ins;
    }

    //overide the pure virtual functions in class: ServiceListener
    // Listener callback to process an add event to the Service
    void ProcessAdd(AlgoExecution &data)  ;

    // Listener callback to process a remove event to the Service
    void ProcessRemove(AlgoExecution&data)  ;

    // Listener callback to process an update event to the Service
    void ProcessUpdate(AlgoExecution&data)  ;

    // return position service
    BondExecutionService* GetBondExecutionService();
};


//define member functions in class: BondExecutionService
ExecutionOrder<Bond>& BondExecutionService::GetData(std::string key)
{
    return execution_data.at(key);
}

void BondExecutionService::OnMessage(ExecutionOrder<Bond> &data)
{
    // no implementation
}

void BondExecutionService::AddListener(ServiceListener<ExecutionOrder<Bond>> *listener)
{
    listeners.push_back(listener);
}

const std::vector< ServiceListener<ExecutionOrder<Bond>>* >& BondExecutionService::GetListeners() const
{
    return listeners;
}

// Add Algo Execution to the service
void BondExecutionService::AddAlgoExecution(AlgoExecution& ae)
{
    //firstly, making the AlgoExecution stored
    std::string productID=ae.GetExecutionOrder().GetProduct().GetProductId();

    //store the AlgoExecution
    execution_data[productID]=ae.GetExecutionOrder();

    //pass the execution data to listeners
    std::cout<<"data goes from BondExecutionService -> listener."<<std::endl;
    auto val=ae.GetExecutionOrder();
    for(auto& l: listeners){
        l->ProcessAdd(val);
    }
}

void BondExecutionService::ExecuteOrder(const ExecutionOrder<Bond>& order, Market market)
{
    //firstly, making the order stored
    std::string productId=order.GetProduct().GetProductId();

    //store
    ExecutionOrder<Bond> new_exe(order);
    execution_data.insert(std::make_pair(productId,new_exe));

    //pass the execution data to listeners
    ExecutionOrder<Bond> eo=execution_data[productId];
    for(auto& l: listeners){
        l->ProcessAdd(eo);
    }
}



//define member functions in class: BondExecutionServiceListener
// Listener callback to process an add event to the Service
void BondExecutionServiceListener::ProcessAdd(AlgoExecution &data)
{
    bond_exe_service->AddAlgoExecution(data);

//    auto val=data.GetExecutionOrder();
//    bond_exe_service->ExecuteOrder(val,ESPEED);
}

// Listener callback to process a remove event to the Service
void BondExecutionServiceListener::ProcessRemove(AlgoExecution&data)
{
    // no implementation
}

// Listener callback to process an update event to the Service
void BondExecutionServiceListener::ProcessUpdate(AlgoExecution&data)
{
    // no implementation
}

// return position service
BondExecutionService* BondExecutionServiceListener::GetBondExecutionService()
{
    return bond_exe_service;
}

#endif