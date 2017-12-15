#include "products.h"
#include "tradebookingservice.h"
#include "positionservice.h"
#include "riskservice.h"
#include "pricingservice.h"
//#include "marketdataservice.h"
#include "historicaldataservice.h"
//#include "executionservice.h"
//#include "bondexecutionservice.h"
//#include "streamingservice.h"
#include "algostreamingservice.h"
//#include "inquiryservice.h"
//#include "support.h"
#include "bondstreamingservice.h"
//#include "guiservice.h"
//
//#include <iostream>
//#include <fstream>
using namespace std;


/**
 * Before Run the code, please pay attention !!!
 * 1. Path1 and Path2 cannot be run at the same time. You can firstly comment Path2 run Path1 and comment Path1 run Path2 to get positions.txt and risk.txt
 * 2. Path3 and Path4 cannot be run at the same time.
 * 3. You can change the number of input of price and marketdata smaller by changing the j value in support.h price_file and market_file in order to run it quickly.
 **/

int main(){
    //write all input data according to the requirement in the note.
    bond_file();
    trade_file();
    prices_file();
    market_file();
    inquiries_file();

    //form each path
/****************************************************************************************/
    //Path1: TradingBookingService -> PositionService -> HistoricalDataService
//    auto tradebookingserviceconnector = TradeBookingConnector::Generate_Instance();
//    auto tradebookingservice = tradebookingserviceconnector->GetService();
//
//    //connect positionservice with tradingbookservice through listener
//    auto positionservicelistener = PositionServiceListener::Generate_Instance();
//    tradebookingservice->AddListener(positionservicelistener);
//
//    auto positionservice = positionservicelistener->GetService();
//
//    //connect bondhistoricalpositionservice with positionservice through listener
//    auto historicalpositionlistener = BondHistoricalPositionServiceListener::Generate_Instance();
//    positionservice->AddListener(historicalpositionlistener);
//
//    //Path1 has been done! Print out the positions.txt
//    tradebookingserviceconnector->Subscribe();


/******************************************************************************************/


    //Path2: TradeBookingService -> PositionService -> RiskService -> HistoricalDataService
    auto bondtradebookingserviceconnector = TradeBookingConnector::Generate_Instance();
    auto bondtradebookingservice = bondtradebookingserviceconnector->GetService();

    //connect positionservice with tradingbookservice through listener
    auto bondpositionservicelistener = PositionServiceListener::Generate_Instance();
    bondtradebookingservice->AddListener(bondpositionservicelistener);

    auto bondpositionservice = bondpositionservicelistener->GetService();

    //connect riskservice with positionservice through listener
    auto bondriskservicelistener = RiskServiceListener::Generate_Instance();
    bondpositionservice->AddListener(bondriskservicelistener);

    auto bondriskservice = bondriskservicelistener->GetRiskService();

    //connect historicalpv01service with positionservice through listener
    auto historicalPV01servicelistener = BondHistoricalPV01ServiceListener::Generate_Instance();
    bondriskservice->AddListener(historicalPV01servicelistener);

    //Path2 has been done! Print out the risk.txt
    bondtradebookingserviceconnector->Subscribe();


/******************************************************************************************/

    //Path3: PricingService -> GuiService
//    auto pricingserviceconnector = PricingServiceConnector::Generate_Instance();
//    auto pricingservice = pricingserviceconnector->GetService();
//
//    //connect guiservice with pricestreamingservice through listener
//    auto guiservicelistener = BondGuiServiceListener::Generate_Instance();
//    pricingservice->AddListener(guiservicelistener);
//
//    //Path3 has been done! Print out the gui.txt
//    pricingserviceconnector->Subscribe();


/******************************************************************************************/

    //Path4: PricingService -> AlgoStreamingService -> StreamingService -> HistoricalDataService
    auto pricingserviceconnector = PricingServiceConnector::Generate_Instance();
    auto pricingservice = pricingserviceconnector->GetService();

    //connect algostreamingservice with pricestreamingservice through listener
    auto algostreamingservicelistener = AlgoStreamingServiceListener::Generate_Instance();
    pricingservice->AddListener(algostreamingservicelistener);

    auto algostreamingservice = algostreamingservicelistener->GetAlgoStreamingService();

    //connect bondstreamingservice with algostreamingservice through listener
    auto bondstreamingservicelistener = BondStreamingServiceListener::Generate_Instance();
    algostreamingservice->AddListener(bondstreamingservicelistener);

    auto bondstreamingservice = bondstreamingservicelistener->GetBondStreamingService();

    //connect historicalstreamingservice with bondstreamingservice through listener
    auto bondhistoricalstreamingservicelistener = BondHistoricalStreamingServiceListener::Generate_Instance();
    bondstreamingservice->AddListener(bondhistoricalstreamingservicelistener);


    //Path4 has been done! Print out the streaming.txt
    pricingserviceconnector->Subscribe();



/******************************************************************************************/

    //Path5: MarketDataService -> AlgoExecutionService -> ExecutionService -> HistoricalDataService
    auto marketdataserviceconnector = MarketDataConnector::Generate_Instance();
    auto marketdataservice = marketdataserviceconnector->GetService();

    //connect algoexecuteservice with pricingservice through listener
    auto algoexecutionservicelistener = AlgoExecutionServiceListener::Generate_Instance();
    marketdataservice->AddListener(algoexecutionservicelistener);

    auto algoexecutionservice = algoexecutionservicelistener->GetAlgoExecutionService();

    //connect executionservice with algoexecutionservice through listener
    auto executionservicelistener = BondExecutionServiceListener::Generate_Instance();
    algoexecutionservice->AddListener(executionservicelistener);

    auto executionservice = executionservicelistener->GetBondExecutionService();

    //connect bondhistoricalexecutionservice with executionservice through listener
    auto historicalexecutionserviceistener = BondHistoricalExecutionServiceListener::Generate_Instance();
    executionservice->AddListener(historicalexecutionserviceistener);

    //Path5 has been done! Print out the executions.txt
    marketdataserviceconnector->Subscribe();


/******************************************************************************************/

    //Path6: InquiryService ->  HistoricalDataService
    auto bondinquiryserviceconnector = BondInquiryServiceConnector::Generate_Instance();
    auto bondinquiryservice = bondinquiryserviceconnector->GetService();

    //connect historicalinquiryservice with inquiryservice through listener
    auto historicalinquiryservicelistener = BondHistoricalInquiryServiceListener::Generate_Instance();
    bondinquiryservice->AddListener(historicalinquiryservicelistener);

    //Path6 has been done! Print out the allinquiries.txt
    bondinquiryserviceconnector->Subscribe();

    return 0;
}

