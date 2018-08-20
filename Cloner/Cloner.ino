/*
   Special thanks to Neil Kolban for his BLE library: https://github.com/nkolban/ESP32_BLE_Arduino
   Our work is built over his library.

  BLEMystique Cloner module by Pentester Academy.
   
  Terms of use:
  - This code must only be used for non-commercial purpose.
  - This repository (https://github.com/pentesteracademy/blemystique) must be mentioned as a reference.  
   
*/


#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 15; //In seconds
int deviceCount = 0;  
BLEScanResults foundDevices;
static BLEAddress *pServerAddress;

std::map<std::string, BLERemoteService*>* pMap;
BLERemoteService* pRemoteService;
std::map<std::string, BLERemoteCharacteristic*>* cMap;
BLERemoteCharacteristic* pRemoteCharacteristic;
BLEClient*  pClient;
BLEServer *pServer;

// Printing message on serial
void listenCommandMsg(){
  Serial.println("\n\nListening for command .......");
}

// COnnecting to a BLE device in client mode
bool connectToServer(BLEAddress pAddress) {
    Serial.printf("Connecting to: %s\n", pAddress.toString().c_str());
    
    pClient  = BLEDevice::createClient();
    Serial.println("Entering client mode.");

    // Connect to the remote BLE Server.
    pClient->connect(pAddress);
    Serial.println("Connected to target device.");
    
    // Enumerating services
    pMap = pClient->getServices();
    Serial.printf("Total services offered: %d\n",pMap->size());

    // Printing services
    int service_count = 0;
    std::map<std::string,BLERemoteService*>::const_iterator it;
    for(it = pMap->begin(); it != pMap->end(); ++it)
    {
      pRemoteService = it->second;

      // Obtain a reference to the service
      if (pRemoteService == nullptr) {
        Serial.print(" No services");
        return false;
      }
      service_count++;
      if (service_count == 1){
        Serial.printf("No. | %36s | %6s |\n","SERVICE UUID","HANDLE");
        printDashLine();
      }
      Serial.printf("%3d | %36s |  %04x  |\n", service_count, pRemoteService->getUUID().toString().c_str(),pRemoteService->getHandle());
    }

    Serial.printf("\n");

    // Printing services
    service_count = 0;
    int char_count = 0;
    for(it = pMap->begin(); it != pMap->end(); ++it)
    {
      // Getting service
      pRemoteService = it->second;
      service_count++;
      Serial.printf("\n");
      Serial.printf("Service no:%3d | UUID: %36s | Handle: %04x \n", service_count, pRemoteService->getUUID().toString().c_str(),pRemoteService->getHandle());
      printDashLine2();
      
      // Getting characteristics map
      cMap = pRemoteService->getCharacteristics();
      //Serial.printf("Characteristics count: %d\n",cMap->size());

      char_count = 0;
      // Iterating over characteristics
      std::map<std::string, BLERemoteCharacteristic*>::const_iterator itr;
      for(itr = cMap->begin(); itr != cMap->end(); ++itr){
        
        pRemoteCharacteristic = itr->second;
        
        if (pRemoteCharacteristic == nullptr) {
             Serial.print("No characteristic ");
             continue;
        }

      char_count++;
      if (char_count == 1){
        Serial.printf("No. | %36s | %s | %s | %s | %s | %s | %s | %s | %20s | %s \n", " CHARACTERISTIC UUID ","HANDLE","B","I","N","R","W","WN", "DESCRIPTORS", "VALUE");
        printDashLine3();
      }

      std::string descriptor;    
      BLERemoteDescriptor* pRemoteDescriptor = pRemoteCharacteristic->getDescriptor(pRemoteCharacteristic->getUUID());
  
      if (pRemoteDescriptor != NULL){
        Serial.printf("%s|\n",pRemoteDescriptor->toString().c_str());
        descriptor = pRemoteDescriptor->toString().c_str();
      }
      std::string value = pRemoteCharacteristic->readValue();
      Serial.printf("%3d | %36s |  %04x  | %d | %d | %d | %d | %d | %2d | %20s | %s \n", char_count, pRemoteCharacteristic->getUUID().toString().c_str(), pRemoteCharacteristic->getHandle(), pRemoteCharacteristic->canBroadcast(), pRemoteCharacteristic->canIndicate(), pRemoteCharacteristic->canNotify(), pRemoteCharacteristic->canRead(), pRemoteCharacteristic->canWrite(), pRemoteCharacteristic->canWriteNoResponse(), descriptor.c_str(), value.c_str());
    }
  }
  
pClient->disconnect();
Serial.println("\n\nDisconnected from target device.");
listenCommandMsg();
}

void printDashLine(){
  Serial.println("-----------------------------------------------------");
}

void printDashLine2(){
  Serial.println("#=================================================================================#");
}

void printDashLine3(){
  Serial.println("------------------------------------------------------------------------------------------------------");
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void parseScannedDevices(BLEAdvertisedDevice advertisedDevice) {
      deviceCount++;
      if (deviceCount == 1){
        Serial.printf("No. | %17s | RSSI | %18s |\n"," MAC ADDRESS "," DEVICE NAME ");
        printDashLine();
      }
      Serial.printf("%3d | %17s | %4d | %18s |\n", deviceCount, advertisedDevice.getAddress().toString().c_str(), advertisedDevice.getRSSI(), advertisedDevice.getName().c_str());
    }
    
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      parseScannedDevices(advertisedDevice);
    }
};

// BLE Scan 
void doScan(){
  deviceCount = 0;
  Serial.println("Starting scan");
  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  foundDevices = pBLEScan->start(scanTime);
  printDashLine();
  Serial.printf("Total devices found: %d\n", foundDevices.getCount());
}

// Connecting to BLE device and reading information
void deepScan(int index_num){
  Serial.printf("Scanning device no. : %d\n", index_num);
  BLEAdvertisedDevice advertisedDevice = foundDevices.getDevice(index_num - 1 );
  pServerAddress = new BLEAddress(advertisedDevice.getAddress());
  if (!connectToServer(*pServerAddress)) {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
}

// Cloner routine
void cloneDevice(int index_num){
  std::map<std::string,BLERemoteService*>::const_iterator t_it;
  BLECharacteristic *pCharacteristic;

  // Getting MAC address
  BLEAdvertisedDevice advertisedDevice = foundDevices.getDevice(index_num - 1);
  BLEAddress pAddress = *(new BLEAddress(advertisedDevice.getAddress()));
  Serial.printf("Connecting to: %s\n", pAddress.toString().c_str());

  // Getting device name
  std::string devName = advertisedDevice.getName();
  //BLEDevice::init(devName.c_str());
  
  Serial.printf("Cloning device: %s\n", devName.c_str());
  
  pClient = BLEDevice::createClient();
  Serial.println("Entering client mode.");

  // Connect to the remote BLE Server.
  pClient->connect(pAddress);
  Serial.println("Connected to target device.");
    
  // Enumerating services
  pMap = pClient->getServices();

  // Creating server
  BLEDevice::init("");
  pServer = BLEDevice::createServer();
  
  BLEService *pService;

  Serial.printf("Services offered: %d\n",pMap->size());
  for(t_it = pMap->begin(); t_it != pMap->end(); ++t_it)
  {
    // Get and create services
    pRemoteService = t_it->second;

    // Ommiting generic services
    std::string uuid_str = pRemoteService->getUUID().toString();
    if(strncmp(uuid_str.c_str(),"00001800",8) == 0 || strncmp(uuid_str.c_str(),"00001801",8) == 0)
      continue;
        
    pService = pServer->createService(pRemoteService->getUUID());
    
    // Getting characteristics map
    cMap = pRemoteService->getCharacteristics();

    std::map<std::string, BLERemoteCharacteristic*>::const_iterator t_itr;
    for(t_itr = cMap->begin(); t_itr != cMap->end(); ++t_itr)
    {   
        pRemoteCharacteristic = t_itr->second;
        Serial.printf("%s  \n", pRemoteCharacteristic->toString().c_str());     

        int propertyBitmap = 0;
        
        if(pRemoteCharacteristic->canBroadcast())
          propertyBitmap = propertyBitmap | BLECharacteristic::PROPERTY_BROADCAST; 

        if(pRemoteCharacteristic->canIndicate())
          propertyBitmap = propertyBitmap | BLECharacteristic::PROPERTY_INDICATE; 
        
        if(pRemoteCharacteristic->canNotify())
          propertyBitmap = propertyBitmap | BLECharacteristic::PROPERTY_NOTIFY; 
        
        if(pRemoteCharacteristic->canRead())
          propertyBitmap = propertyBitmap | BLECharacteristic::PROPERTY_READ; 
        
        if(pRemoteCharacteristic->canWrite())
          propertyBitmap = propertyBitmap | BLECharacteristic::PROPERTY_WRITE; 
        
        if(pRemoteCharacteristic->canWriteNoResponse())
          propertyBitmap = propertyBitmap | BLECharacteristic::PROPERTY_WRITE_NR; 
          
        // Set characteristics
        pCharacteristic = pService->createCharacteristic(
                                         pRemoteCharacteristic->getUUID(),
                                         propertyBitmap
                                       );

        // Setting value
        std::string value = pRemoteCharacteristic->readValue();
        pCharacteristic->setValue(value);

        // Setting descriptors
        BLERemoteDescriptor* pRemoteDescriptor = pRemoteCharacteristic->getDescriptor(pRemoteCharacteristic->getUUID());
        if (pRemoteDescriptor != NULL){
          Serial.printf("%s|\n",pRemoteDescriptor->toString().c_str());
          BLEDescriptor pDescriptor = BLEDescriptor(pRemoteDescriptor->getUUID());
          std::string dValue = pRemoteDescriptor->toString().c_str();
          pDescriptor.setValue(dValue);
          pCharacteristic->addDescriptor(&pDescriptor);
        }
    }
    // Start service
    pService->start();
  }
  pClient->disconnect();
  Serial.println("\n\nDisconnected from target device.");

  // Starting advertising
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();

  // NISH TODO  print MAC of local machine
  //BLEAddress lAddress = *(new BLEAddress(BLEDevice::getAddress()));
  //Serial.println("Cloned device operating on MAC: %s", lAddress.toString().c_str());
  Serial.println("Cloned device operating.");
}

void onButton(){
    doScan();
    listenCommandMsg();
}

void setup() {
  Serial.begin(115200);
  Serial.println("Intializing device");
  BLEDevice::init("");
  doScan();
  listenCommandMsg();
}

int handleDoubleDigits(){
  char first_letter;
  int index_num;
  first_letter = Serial.read();
  index_num = (int) first_letter;
  index_num = index_num - 48;
  while(1){
    char temp_letter = Serial.read();
    int temp_num = (int) temp_letter;
    temp_num = temp_num - 48;
    if (temp_num >= 0 && temp_num <= 9){
      index_num = index_num * 10 + temp_num;
    } else 
      break;
  }
  Serial.printf("Chosen target : %d \n", index_num);
  return index_num;
}

void handleSerialCommands(){
  char first_letter = Serial.read();
    if (first_letter == 'T' || first_letter == 't'){
        int index_num = handleDoubleDigits();
        deepScan(index_num);
    }
    if (first_letter == 'C' || first_letter == 'c'){
        int index_num = handleDoubleDigits();
        cloneDevice(index_num);
        listenCommandMsg();
    }
    if (first_letter == 'S' || first_letter == 's'){
      doScan();
      listenCommandMsg();
    }
}

// Main loop
void loop() {
  // put your main code here, to run repeatedly:
    static uint8_t lastPinState = 1;
    uint8_t pinState = digitalRead(0);
    if(!pinState && lastPinState){
        onButton();
    }
    lastPinState = pinState;
    handleSerialCommands();
}
