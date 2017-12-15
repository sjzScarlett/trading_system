/* This is the class works for define the information for each bond and support services
 * @Author: Sijia Zhang
 */

#ifndef SUPPORT_HPP
#define SUPPORT_HPP

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "products.h"


//CUSIPS
const std::string cusip1_year_2  = "912828F62";
const std::string cusip2_year_3  = "9128283G3";
const std::string cusip3_year_5  = "9128283C2";
const std::string cusip4_year_7  = "9128283D0";
const std::string cusip5_year_10 = "9128283F5";
const std::string cusip6_year_30 = "912810RZ3";

const std::vector<std::string> CUSIPS_CONTAINER = {cusip1_year_2, cusip2_year_3, cusip3_year_5,
        cusip4_year_7, cusip1_year_2, cusip5_year_10, cusip6_year_30};


std::vector<float> COUPON_CONTAINER = {
        static_cast<float>(0.015),
        static_cast<float>(0.0175),
        static_cast<float>(0.02),
        static_cast<float>(0.0225),
        static_cast<float>(0.0255),
        static_cast<float>(0.0175)
};

const std::vector<date> MATURITY_CONTAINER{
        date(2019, 10, 31),
        date(2020, 11, 15),
        date(2022, 10, 31),
        date(2024, 11, 15),
        date(2027, 11, 15),
        date(2047, 11, 15)
};

std::vector<Bond> bond_container;

void bond_file() {

    // Create a Services to contain data
    auto bondProductService = BondProductService::Generate_Instance();
    auto bondPositionService = PositionService::Generate_Instance();
    auto bondRiskService = RiskService::Generate_Instance();

    for (int i = 0; i < 6; ++i) {
        Bond bond(CUSIPS_CONTAINER[i], CUSIP, "T", COUPON_CONTAINER[i], MATURITY_CONTAINER[i]);
        Position<Bond> position(bond);
        PV01<Bond> pv01(bond, rand() % 1 / 100000., position.GetAggregatePosition());
        bondProductService->AddBond(bond);


        bondPositionService->Addpos(position);
        bondRiskService->AddRisk(pv01);
        bond_container.push_back(bond);
    }
}


//define a function for create Bond
Bond GetBond(std::string cuips){
    bond_file();

    for(auto it : bond_container){
        return it;
    }
}



//Transform price
auto PriceIndex = [](int num) {
    //using the same way we define in generate price in trade
    int num1 = num / 256, num2 = num % 256, num3 = num2 / 8, num4 = num2 % 8;
    string str1 = std::to_string(99 + num1) + "-", str2 = std::to_string(num3), str3 = std::to_string(num4);

    if (num3 < 10) str2 = "0" + str2;
    if (num4 == 4)	str3 = "+";
    return str1 + str2 + str3;
};



//define a function to generate trades.txt
void trade_file() {
    ofstream os("../input/trades.txt");
    os << "CUSIP,Trade_ID,Book,Price,Quantity,Side\n";
    for (int i = 1; i <= 6; ++i) {
        //we put the trade which has same productID together
        std::string CUS_IP = CUSIPS_CONTAINER[i - 1];

        for (int j = 1; j <= 10; ++j) {
            //define numbers used to define price
            int num, num1, num2, num3, num4;
            std::string str1, str2, str3;
            num = rand() % (256 * 2 + 1);
            num1 = num / 256;

            num2 = num % 256;
            num3 = num2 / 8;

            num4 = num2 % 8;

            str1 = std::to_string(99 + num1) + "-";
            str2 = std::to_string(num3);
            str3 = std::to_string(num4);

            //define the decimals
            if (num4 == 4) str3 = "+";
            if (num3 < 10) str2 = "0" + str2;

            //define quantity, need to cycle between 1000000 to 5000000
            int quantity=0;
            if(j<=5){
                quantity=1000000*j;
            }
            else{
                quantity=(j-5)*1000000;
            }

            //input
            os << CUS_IP << ",T" << (i - 1) * 10 + j << ",TRSY" << 1 + rand() % 3
               << "," << str1 + str2 + str3 << "," << std::to_string(quantity) << ","
               << (rand() % 2 == 1 ? "BUY" : "SELL") << std::endl;
        }
    }
}



//generate price
void prices_file() {
    ofstream os("../input/prices.txt");
    os << "CUSIP,Mid_Price,Bid/Offer_Spread\n";
    for (int i = 1; i <= 6; ++i) {
        std::string CUS_IP = CUSIPS_CONTAINER[i - 1];
        for (int j = 1; j <= 1000000; ++j){
            //define mid price
            int mid_price = rand() % (256 * 2 - 8) + 4;

            //define bid/offer spread
            int tmp = (rand() % 3 + 2);
            std::string osc_str = "0-00" + (tmp == 4 ? "+" : std::to_string(tmp));
            os << CUS_IP << "," << PriceIndex(mid_price) << ',' << osc_str << endl;
        }
    }
}


void market_file() {
    ofstream os("../input/marketdata.txt");
    os << "CUSIP,bidprice1,quantity,bidprice2,quantity,bidprice3,quantity,bidprice4,quantity,bidprice5,quantity,offerprice1,quantity,offerprice2,quantity,offerprice3,quantity,offerprice4,quantity,offerprice5,quantity,\n";
    for (int i = 1; i <= 6; ++i) {
        std::string CUS_IP = CUSIPS_CONTAINER[i-1];
        for (int j = 1; j <= 1000000; ++j) {
            //print cusip
            os << CUS_IP << ',';

            //define mid_price
            int mid_price = rand() % (256 * 2 + 1);


            //define bid and ask price following the principle that ascend from 1/128 to 1/32 and back to 1/128
            int bid_price = mid_price - 1;
            int offer_price = mid_price + 1;
            for (int k = 1; k <= 5; ++k) {
                int quantity = 1000000 * k;

                if(k<=4){
                    os << PriceIndex(bid_price--)<<','<<quantity<<',';
                }
                else{
                    os<< PriceIndex(mid_price - 1)<<','<<quantity<<',';
                }


            }
            for (int k = 1; k <= 5; ++k) {
                int quantity = 1000000 * k;

                if(k<=4){
                    os << PriceIndex(offer_price++)<<','<<quantity<<',';
                }
                else{
                    os << PriceIndex(mid_price + 1)<<','<<quantity<<',';
                }
            }
            os << endl;
        }
    }
}


void inquiries_file() {
    ofstream os("../input/inquiries.txt");
    os << "CUSIP, side, quantity, price, state\n";
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 6; ++j) {
            // CUSIP_CODE, SIDE, QUANTITY, PRICE, STATE
            os << CUSIPS_CONTAINER[j] + ',' + (rand() % 2 == 0 ? "BUY" : "SELL") + ',' + "99.35" + ',' + std::to_string(rand() % 100 * i + 125) + ',' + "RECEIVED" << endl;
        }
    }
}
#endif
