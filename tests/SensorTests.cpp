#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "SensorServer.hpp"
#include "TestObject.hpp"
#include "fakeit.hpp"

using namespace std;
using namespace fakeit;

TEST_CASE("Server decodes for a TempSensor") {
   SensorServer server;
   server.initializeDecoders();
   
   vector<uint8_t> EncodedMessage = {0,1,23};

   server.Decoder(EncodedMessage);
   vector <SensorValue*> decodedValues = server.getDecodedValues();

   REQUIRE(decodedValues.at(0) != nullptr);
   REQUIRE(decodedValues.size() == 1);
   CHECK(decodedValues.at(0)->getValue() == 23.00);
   CHECK(decodedValues.at(0)->getType() == "Temperature");
   CHECK(decodedValues.at(0)->getUnit() == "Celsius");
}

TEST_CASE("Server decodes for a MultiReadingTemperatureSensor") {
   SensorServer server;
   server.initializeDecoders();

   vector<uint8_t> EncodedMessage = {0,3,3,0,23,0,24,0,25};

   server.Decoder(EncodedMessage);
   vector <SensorValue*> decodedValues = server.getDecodedValues();

   REQUIRE(decodedValues.size() == 3);

   CHECK(decodedValues.at(0)->getValue() == 23.00);
   CHECK(decodedValues.at(0)->getType() == "Temperature");
   CHECK(decodedValues.at(0)->getUnit() == "Celsius");

   CHECK(decodedValues.at(1)->getValue() == 24.00);
   CHECK(decodedValues.at(1)->getType() == "Temperature");
   CHECK(decodedValues.at(1)->getUnit() == "Celsius");

   CHECK(decodedValues.at(2)->getValue() == 25.00);
   CHECK(decodedValues.at(2)->getType() == "Temperature");
   CHECK(decodedValues.at(2)->getUnit() == "Celsius");
}

TEST_CASE("Server decodes for a TempSensor in Kelvin") {
   SensorServer server;
   server.initializeDecoders();

   vector<uint8_t> EncodedMessage = {0,2,1,44}; //1..00101100 - 300 in binary

   server.Decoder(EncodedMessage);
   vector <SensorValue*> decodedValues = server.getDecodedValues();

   REQUIRE(decodedValues.size() == 1);

   CHECK(decodedValues.at(0)->getValue() == 27.00);
   CHECK(decodedValues.at(0)->getType() == "Temperature");
   CHECK(decodedValues.at(0)->getUnit() == "Celsius");
}

TEST_CASE("Server decodes for a Airpressure in Pascal"){
   SensorServer server;
   server.initializeDecoders();

   vector<uint8_t> EncodedMessage{0x00,0x04,0x00,0x65}; //101Pa

   server.Decoder(EncodedMessage);
   vector<SensorValue*> decodedValues = server.getDecodedValues();

   REQUIRE(decodedValues.size() == 1);
   REQUIRE(decodedValues.at(0) != nullptr);
   CHECK(decodedValues.at(0)->getValue() == 101);
   CHECK(decodedValues.at(0)->getType() == "Airpressure");
   CHECK(decodedValues.at(0)->getUnit() == "Pascal");
}

TEST_CASE("decode scenario for 2 MultiReadingTemperatureDecoder messages") {
   SensorServer server;
   server.initializeDecoders();

   vector<uint8_t> EncodedMessage1 = {0,3,3,0,23,0,24,0,25};
   vector<uint8_t> EncodedMessage2 = {0,3,3,0,26,0,27,0,28};

   server.Decoder(EncodedMessage1);
   server.Decoder(EncodedMessage2);

   vector<SensorValue*> decodedValues = server.getDecodedValues();
   REQUIRE(decodedValues.size() == 6);
   CHECK(decodedValues.at(0) != nullptr);
   CHECK(decodedValues.at(0)->getValue() == 23.00);
   CHECK(decodedValues.at(1)->getValue() == 24.00);
   CHECK(decodedValues.at(2)->getValue() == 25.00);
   CHECK(decodedValues.at(3)->getValue() == 26.00);
   CHECK(decodedValues.at(4)->getValue() == 27.00);
   CHECK(decodedValues.at(5)->getValue() == 28.00);
}

TEST_CASE("Sensor disconnect scenario for MultiReadingTemperatureDecoder with test double") {
   SensorServer server;
   TestSensor sensor;

   vector<uint8_t> EncodedMessage1 = {0,3,3,0,23,0,24,0,25};
   vector<uint8_t> EncodedMessage2 = {0,3,3,0,26,0,27,0,28};

   sensor.addDataToBuffer(EncodedMessage1);
   sensor.addDataToBuffer(EncodedMessage2);

   vector<vector<uint8_t>> bufferedData = sensor.sendBufferData();

   server.initializeDecoders();

   for(auto data : bufferedData){
      server.Decoder(data);
   } 

   vector<SensorValue*> decodedValues = server.getDecodedValues();
   REQUIRE(decodedValues.size() == 6);
   CHECK(decodedValues.at(0) != nullptr);
   CHECK(decodedValues.at(0)->getValue() == 23.00);
   CHECK(decodedValues.at(1)->getValue() == 24.00);
   CHECK(decodedValues.at(2)->getValue() == 25.00);
   CHECK(decodedValues.at(3)->getValue() == 26.00);
   CHECK(decodedValues.at(4)->getValue() == 27.00);
   CHECK(decodedValues.at(5)->getValue() == 28.00);
}

TEST_CASE("Server decodes for a CombinedSensor") {
   SensorServer server;
   server.initializeDecoders();
   
   vector<uint8_t> EncodedMessage = {0,5,0,1,23,0,4,0,101};

   server.Decoder(EncodedMessage);
   vector <SensorValue*> decodedValues = server.getDecodedValues();

   REQUIRE(decodedValues.at(0) != nullptr);
   REQUIRE(decodedValues.at(1) != nullptr);
   REQUIRE(decodedValues.size() == 2);

   CHECK(decodedValues.at(0)->getValue() == 23.00);
   CHECK(decodedValues.at(0)->getType() == "Temperature");
   CHECK(decodedValues.at(0)->getUnit() == "Celsius");
   CHECK(decodedValues.at(1)->getValue() == 101);
   CHECK(decodedValues.at(1)->getType() == "Airpressure");
   CHECK(decodedValues.at(1)->getUnit() == "Pascal");

}