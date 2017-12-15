cmake_minimum_required(VERSION 3.7)
project(final_sijia)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp soa.h products.h tradebookingservice.h pricingservice.h positionservice.h riskservice.h marketdataservice.h executionservice.h streamingservice.h inquiryservice.h historicaldataservice.h support.h algoexecutionservice.h bondexecutionservice.h bondstreamingservice.h algostreamingservice.h guiservice.h)
add_executable(final_sijia ${SOURCE_FILES})