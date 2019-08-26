// ========================================================================================
// Description:       Write data to a sd card
// ========================================================================================

void InitSdCard() {

  SD.begin(mChipSelect);

  if(!SD.exists(mGuestList)){
    Serial.println("Create file...");
    
    File dataFile = SD.open(mGuestList, FILE_WRITE);
    dataFile.close();
    
    Serial.println("...finish");
  }
  else {
    Serial.println("the guest list exist");
  }
  
  if(!mCard.init(SPI_HALF_SPEED, mChipSelect)) {          // initialize sd card and check insert sd card
    Serial.println("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
    Serial.println("\t\tERROR: can not init sd card");
    Serial.println("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
    return;
  }

  if(!mVolume.init(mCard)) {                              // initialize sd card
    Serial.println("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
    Serial.println("\t\tERROR: Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    Serial.println("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
    return;
  }
  
  Serial.println("SD Card: Exist and partion was found");
  SdCardShowSDCardType();                                 // Show sd card information
  SdCardShowVolumeSize();                                 // Show sd card storage size informations
  
}

// ========================================================================================
// Read only the type and print to serial
void SdCardShowSDCardType() {
  
  Serial.print("\nCard type: ");
  
  switch (mCard.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }
}

// ========================================================================================
// print the type and size of the first FAT-type volume
void SdCardShowVolumeSize() {

  uint32_t volumesize;
  Serial.print("\nVolume type is FAT");
  Serial.println(mVolume.fatType(), DEC);
  Serial.println();

  volumesize = mVolume.blocksPerCluster();            // clusters are collections of blocks
  volumesize *= mVolume.clusterCount();               // we'll have a lot of clusters
  volumesize *= 512;                                  // SD card blocks are always 512 bytes
  Serial.print("Volume size (bytes): ");
  Serial.println(volumesize);
  Serial.print("Volume size (Kbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);

  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  mRoot.openRoot(mVolume);

  mRoot.ls(LS_R | LS_DATE | LS_SIZE);                 // list all files in the card with date and size
}

void WriteGuest(String guestName, String setupTime) {

  File dataFile = SD.open(mGuestList, FILE_WRITE);

  if(!dataFile){
    mErrorMessage = "can not open save file";
    mShowErrorMessage = true;
    return;
  }

  String row = "<div class=\"divTableRow\">";
  row += "<div class=\"divTableCell\"> " + guestName + " </div>";
  row += "<div class=\"divTableCell\"> " + setupTime + " </div>";
  row += "</div>";

  Serial.println(row);
  dataFile.println(row);
  dataFile.close();
}

void ReadVisitorsAndPrintToWeb(WiFiClient webclient){
  File file = SD.open(mGuestList);

  if(!file) {
    mErrorMessage = "can not read guest list data";
    mShowErrorMessage = true;
    return;
  }

  webclient.println("  <div class=\"divTableRow\">");
  webclient.println("   <div class=\"divTableCell\">Visitors</div>");
  webclient.println("   <div class=\"divTableCell\">");
  webclient.println(mVisitorsCount, DEC);
  webclient.println("   </div>");
  webclient.println("  </div>");
  webclient.println("  <div class=\"divTableRow\">");
  webclient.println("   <div class=\"divTableCell\">Name:</div>");
  webclient.println("   <div class=\"divTableCell\">visit time:</div>");
  webclient.println("  </div>");

  int visitorsCount = 0;
  while(file.available()) {
    webclient.println(file.readStringUntil('\n'));
    visitorsCount++;
  }
  
  mVisitorsCount = visitorsCount;
  file.close();
}
