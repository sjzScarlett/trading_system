/**
 * algoexecutionservice.h
 * Define the Bond Algo Execution and Service for data comes from MarketDataService.
 *
 * @author Sijia Zhang
 */
#ifndef ALGO_EXECUTION_SERVICE_HPP
#define ALGO_EXECUTION_SERVICE_HPP

#include <string>
#include "soa.h"
#include "marketdataservice.h"
#include "executionservice.h"

/**
 * AlgoExecution is aimed to provide support for AlgoExecutionService.
 * We use the type Bond instead of using template T.
 */

class AlgoExecution{

private:

    //define execution_order in order to pass best offer and bid order in it
    ExecutionOrder<Bond> execution_order;

public:

    //destructor
    ~AlgoExecution(){};

    //ctor
    AlgoExecution(){};

    //ctor
    AlgoExecution(OrderBook<Bond>& ob);

    // OrderChoosing is used to choose the order which has minimum spread(1/128)
    void OrderChoosing(const OrderBook<Bond>& ob);

    // Get Execution order
    ExecutionOrder<Bond> GetExecutionOrder() const;
};


/**
 * AlgoExecutionService is aimed to pick up the best orders from each orderbook in Market data and pass it to the ExecutionService
 * We use the type Bond instead of using template T.
 */

class AlgoExecutionService : public Service<std::string, AlgoExecution>{

private:

    //define listener
    std::vector<ServiceListener<AlgoExecution>*> listeners;

    //define a map to find data on the service
    std::map<std::string, AlgoExecution> algo_execution_data;

    //ctor
    AlgoExecutionService(){};

public:

    // Generate instance
    static AlgoExecutionService* Generate_Instance(){
        //we define this function to generate instance for class MarketDataService.
        static AlgoExecutionService ins;
        return &ins;
    }

    //  pure virtual member functions in class Service.
    // Get data on our service given a key
    AlgoExecution& GetData(std::string key)  ;

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(AlgoExecution &data)  ;

    // Add a listener to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<AlgoExecution> *listener)  ;

    // Get all listeners on the Service.
    const std::vector< ServiceListener<AlgoExecution>* >& GetListeners() const  ;

    // Add an orderbook to the service
    void AddOrderBook(OrderBook<Bond>& ob);

};


/**
 * AlgoExecutionServiceListener passes the order from orderbook to order execution
 * We use the type Bond instead of using template T.
 */

class AlgoExecutionServiceListener : public ServiceListener<OrderBook<Bond>>{

private:

    //define a pointer of AlgoExecutionService
    AlgoExecutionService * algo_exe_service;

    //ctor
    AlgoExecutionServiceListener(){
        algo_exe_service=AlgoExecutionService::Generate_Instance();
    }

public:

    // Generate instance
    static AlgoExecutionServiceListener* Generate_Instance(){
        //we define this function to generate instance for class AlgoExecutionServiceListener.
        static AlgoExecutionServiceListener ins;
        return &ins;
    }

    //overide the pure virtual functions in class: ServiceListener
    // Listener callback to process an add event to the Service
    void ProcessAdd(OrderBook<Bond> &data)  ;

    // Listener callback to process a remove event to the Service
    void ProcessRemove(OrderBook<Bond> &data)  ;

    // Listener callback to process an update event to the Service
    void ProcessUpdate(OrderBook<Bond> &data)  ;

    // return algo execution service
    AlgoExecutionService* GetAlgoExecutionService();

};


/*******************************************AlgoExecutionServiceListener:*************************************************/
//define member functions in class: AlgoExecution
AlgoExecution::AlgoExecution(OrderBook<Bond> &ob)
{
    // define every input parameters in an orderbook
    auto product = ob.GetProduct(); //product

    //side
    int val=0;
    PricingSide side=((val%2)? BID :OFFER); //just initialize orderbook

    //orderID
    std::string orderID="OI"+std::to_string(val);

    //order type
    OrderType ot;
    if(val%5==0){
        ot=FOK;
    }
    else if(val%5==1){
        ot=MARKET;
    }
    else if(val%5==2){
        ot=LIMIT;
    }
    else if(val%5==3){
        ot=STOP;
    }
    else{
        ot=IOC;
    }

    //price and visible quantity
    double price=0.;
    long vq=1000000;

    auto ask = ob.GetOfferStack().begin();
    auto bid = ob.GetBidStack().begin();

    if(side==BID){
        price=ask->GetPrice();
        vq=ask->GetQuantity();

    }
    else if(side==OFFER){
        price=ask->GetPrice();
        vq=ask->GetQuantity();
    }

    //hiden quantity
    long hq=2000000; //initialize to be zero

    //parent ID
    std::string parentID="ParentID"+std::to_string(val);

    //isChild
    bool ischild=false;
    if(val%2==0){
        ischild=true;
    }

    //Then, we can create the execution order!
    ExecutionOrder<Bond> ex_initial(product,side,orderID,ot,price,vq,hq,parentID,ischild);
    execution_order=ex_initial;
}


void AlgoExecution::OrderChoosing(const OrderBook<Bond>& ob)
{
    // This is the member function to select the order has minimum spread, i.e. 1/128
    if(ob.GetProduct().GetProductId()!=execution_order.GetProduct().GetProductId()){
        return; //If we cannot find the exact id, then we cannot choose our execution order
    }
    else{
        //using the same way we initial our Execution Order, just pay attention to choose the order has minimum spread from orderbook
        // define every input parameters in an orderbook
        auto product = ob.GetProduct(); //product

        //side
        static int val=0; //making each time we go through orderbooks, the val will increase by 1
        ++val;
        PricingSide side=((val%2)? BID :OFFER); //just initialize orderbook

        //orderID
        std::string orderID="OrderID"+std::to_string(val);

        //order type
        OrderType ot;
        if(val%5==0){
            ot=FOK;
        }
        else if(val%5==1){
            ot=MARKET;
        }
        else if(val%5==2){
            ot=LIMIT;
        }
        else if(val%5==3){
            ot=STOP;
        }
        else{
            ot=IOC;
        }

        //price and visible quantity
        double price=0.;
        long vq= (1+rand()%2)*1000000;
        if(side==BID){
            //using the same of we define GetBestBidOffer, we choose bid or offer has minimum spread
            int index=0;

            //using loop to find the bid and offer with minimum spread: 1/128
            for(int i=0;i<ob.GetBidStack().size();i++){
                if(ob.GetOfferStack()[i].GetPrice()-ob.GetBidStack()[i].GetPrice()==1/128.){
                    index=i;
                }
            }

            price=ob.GetBidStack()[index].GetPrice();
            vq=ob.GetBidStack()[index].GetQuantity();

        }
        else if(side==OFFER){
            //using the same of we define GetBestBidOffer, we choose bid or offer has minimum spread
            int index=0;

            //using loop to find the bid and offer with minimum spread: 1/128
            for(int i=0;i<ob.GetBidStack().size();i++){
                if(ob.GetOfferStack()[i].GetPrice()-ob.GetBidStack()[i].GetPrice()==1/128.){
                    index=i;
                }
            }

            price=ob.GetOfferStack()[index].GetPrice();
            vq=ob.GetOfferStack()[index].GetQuantity();
        }

        //hiden quantity
        long hq=2*vq; //initialize to be zero

        //parent ID
        std::string parentID="ParentID"+std::to_string(val);

        //isChild
        bool ischild=false;
        if(val%2==0){
            ischild=true;
        }

        //Then, we can create the execution order!
        ExecutionOrder<Bond> ex_initial(product,side,orderID,ot,price,vq,hq,parentID,ischild);
        execution_order=ex_initial;
    }
}

ExecutionOrder<Bond> AlgoExecution::GetExecutionOrder() const
{
    return execution_order;
}



//define member functions in class: AlgoExecutionService
void AlgoExecutionService::AddOrderBook(OrderBook<Bond>& od)
{
    //firstly, making the orderbook stored
    std::string productId=od.GetProduct().GetProductId();

    //store the order
    if(algo_execution_data.find(productId)!=algo_execution_data.end()){
        //if we can find the key of algo_execution_data, then just choose the order which has smallest spread
        algo_execution_data[productId].OrderChoosing(od);
    }
    else{
        //make a new pair
        AlgoExecution new_algo(od);
        algo_execution_data.insert(std::make_pair(productId,new_algo));
    }

    //pass the execution data to listeners
    std::cout<<"data goes from AlgoExecutionService -> listener."<<std::endl;
    AlgoExecution ae=algo_execution_data[productId];
    for(auto& l: listeners){
        l->ProcessAdd(ae);
    }
}

AlgoExecution& AlgoExecutionService::GetData(std::string key)
{
    return algo_execution_data.at(key);
}

void AlgoExecutionService::OnMessage(AlgoExecution &data)
{
    // no implementation
}

void AlgoExecutionService::AddListener(ServiceListener<AlgoExecution> *listener)
{
    listeners.push_back(listener);
}

const std::vector< ServiceListener<AlgoExecution>* >& AlgoExecutionService::GetListeners() const
{
    return listeners;
}



//define member functions in class: AlgoExecutionServiceListener
void AlgoExecutionServiceListener::ProcessAdd(OrderBook<Bond> &data)
{
    algo_exe_service->AddOrderBook(data);
}

// Listener callback to process a remove event to the Service
void AlgoExecutionServiceListener::ProcessRemove(OrderBook<Bond> &data)
{
    // no implementation
}

// Listener callback to process an update event to the Service
void AlgoExecutionServiceListener::ProcessUpdate(OrderBook<Bond> &data)
{
    // no implementation
}

// return position service
AlgoExecutionService* AlgoExecutionServiceListener::GetAlgoExecutionService()
{
    return algo_exe_service;
}

#endif