int getFingerprintIDez();
uint8_t getFingerprintEnroll(int id);

// pin #0 = D3 is IN from sensor (GREEN wire)
// pin #2 = D4 is OUT from arduino  (WHITE wire)
SoftwareSerial mySerial(0, 2);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setupFingerprint() {
  Serial_println("fingertest");
  finger.begin(57600); // set the data rate for the sensor serial port
  if (finger.verifyPassword()) {
    Serial_println("Found fingerprint sensor!");
  } else {
    Serial_println("Did not find fingerprint sensor :(");
    while (1);
  }
  Serial_println("Waiting for valid finger...");
}

int get_ID_Card(){
  ID_Card = getFingerprintIDez();
  return ID_Card;
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial_println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial_println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial_println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial_println("Imaging error");
      return p;
    default:
      Serial_println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial_println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial_println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial_println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial_println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial_println("Could not find fingerprint features");
      return p;
    default:
      Serial_println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial_println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial_println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial_println("Did not find a match");
    return p;
  } else {
    Serial_println("Unknown error");
    return p;
  }

  // found a match!
  Serial_print("Found ID #"); Serial_print(finger.fingerID);
  Serial_print(" with confidence of "); Serial_println(finger.confidence);
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() { // <<<<<<<< Check finger scan
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial_print("Found ID #"); Serial_print(finger.fingerID);
  Serial_print(" with confidence of "); Serial_println(finger.confidence);
  //ID_Card = finger.fingerID;
  return finger.fingerID;
}


//                      vvvv Add Finger vvvv

void Add_finger(int ID_add)                     // run over and over again
{
  Serial_println("Type in the ID # you want to save this finger as...");
  int id = 0;
  while (true) {
    char c = ID_add;
    if (! isdigit(c)) break;
    id *= 10;
    id += c - '0';
  }
  Serial_print("Enrolling ID #");
  Serial_println(id);

  while (!  getFingerprintEnroll(id) );
}

uint8_t getFingerprintEnroll(int id) {
  int p = -1;
  Serial_println("Waiting for valid finger to enroll");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial_println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial_println(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial_println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial_println("Imaging error");
        break;
      default:
        Serial_println("Unknown error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial_println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial_println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial_println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial_println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial_println("Could not find fingerprint features");
      return p;
    default:
      Serial_println("Unknown error");
      return p;
  }

  Serial_println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }

  p = -1;
  Serial_println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial_println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial_print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial_println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial_println("Imaging error");
        break;
      default:
        Serial_println("Unknown error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial_println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial_println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial_println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial_println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial_println("Could not find fingerprint features");
      return p;
    default:
      Serial_println("Unknown error");
      return p;
  }


  // OK converted!
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial_println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial_println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial_println("Fingerprints did not match");
    return p;
  } else {
    Serial_println("Unknown error");
    return p;
  }

  Serial_print("ID "); Serial_println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial_println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial_println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial_println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial_println("Error writing to flash");
    return p;
  } else {
    Serial_println("Unknown error");
    return p;
  }
}


//                      ^^^^ Add Finger ^^^^
