TCPClient client;

void ipArrayFromString(byte ipArray[], String ipString) {
  int dot1 = ipString.indexOf('.');
  ipArray[0] = ipString.substring(0, dot1).toInt();
  int dot2 = ipString.indexOf('.', dot1 + 1);
  ipArray[1] = ipString.substring(dot1 + 1, dot2).toInt();
  dot1 = ipString.indexOf('.', dot2 + 1);
  ipArray[2] = ipString.substring(dot2 + 1, dot1).toInt();
  ipArray[3] = ipString.substring(dot1 + 1).toInt();
}

int connectToMyServer(String params) {
  //parse data
  int colonIndex = params.indexOf(":");
  String ip = params.substring(0, colonIndex);
  String port = params.substring(colonIndex+1, params.length());

  byte serverAddress[4];
  ipArrayFromString(serverAddress, ip);
  int serverPort = port.toInt();
  if (client.connect(serverAddress, serverPort)) {
    return 1; // successfully connected
  } else {
    return -1; // failed to connect
  }
}

void setup() {
  Spark.function("connect", connectToMyServer);


}

void loop() {
  if (client.connected()) {
    if (client.available()) {
      // parse and execute commands

      int action = client.read();
      if(DEBUG)
        Serial.println("Action received: "+('0'+action));
      char charVal;
      int pin, mode, val, type, speed, address, stop;
      switch (action) {
        case 0x00:  // pinMode
          pin = client.read();
          mode = client.read();
          //mode is modeled after Standard Firmata
          if (mode == 0x00) {
            pinMode(pin, INPUT);
          } else if (mode == 0x02) {
            pinMode(pin, INPUT_PULLUP);
          } else if (mode == 0x03) {
            pinMode(pin, INPUT_PULLDOWN);
          } else if (mode == 0x01) {
            pinMode(pin, OUTPUT);
          }
          break;
        case 0x01:  // digitalWrite
          pin = client.read();
          val = client.read();
          digitalWrite(pin, val);
          break;
        case 0x02:  // analogWrite
          pin = client.read();
          val = client.read();
          analogWrite(pin, val);
          break;
        case 0x03:  // digitalRead
          pin = client.read();
          val = digitalRead(pin);
          client.write(0x03);
          client.write(pin);
          client.write(val);
          break;
        case 0x04:  // analogRead
          pin = client.read();
          val = analogRead(pin);
          client.write(0x04);
          client.write(pin);
          client.write(val);
          break;


        // Serial API
        case 0x10:  // serial.begin
           type = client.read();
           speed = client.read();
          if (type == 0) {
            Serial.begin(SerialSpeed[speed]);
          } else {
            Serial1.begin(SerialSpeed[speed]);
          }
          break;
        case 0x12:  // serial.end
          type = client.read();
          if (type == 0) {
            Serial.end();
          } else {
            Serial1.end();
          }
          break;
        case 0x13:  // serial.peek
          type = client.read();
          if (type == 0) {
            val = Serial.peek();
          } else {
            val = Serial1.peek();
          }
          client.write(0x07);
          client.write(type);
          client.write(val);
          break;
        case 0x14:  // serial.available()
          type = client.read();
          if (type == 0) {
            val = Serial.available();
          } else {
            val = Serial1.available();
          }
          client.write(0x07);
          client.write(type);
          client.write(val);
          break;
        case 0x15:  // serial.write
          type = client.read();
          len = client.read();
          while (i = 0; i < len; i++) {
            if (type ==0) {
              Serial.write(client.read());
            } else {
              Serial1.write(client.read());
            }
          }
          break;
        case 0x16: // serial.read
          type = client.read();
          if (type == 0) {
            val = Serial.read();
          } else {
            val = Serial1.read();
          }
          client.write(0x16);
          client.write(type);
          client.write(val);
          break;
        case 0x17: // serial.flush
          type = client.read();
          if (type == 0) {
            Serial.flush();
          } else {
            Serial1.flush();
          }
          break;


        // SPI API
        case 0x20:  // SPI.begin
          SPI.begin();
          break;
        case 0x21:  // SPI.end
          SPI.end();
          break;
        case 0x22:  // SPI.setBitOrder
          type = client.read();
          SPI.setBitOrder((type ? MSBFIRST : LSBFIRST));
          break;
        case 0x22:  // SPI.setClockDivider
          val = client.read();
          if (val == 0) {
            SPI.setClockDivider(SPI_CLOCK_DIV2);
          } else if (val == 1) {
            SPI.setClockDivider(SPI_CLOCK_DIV4);
          } else if (val == 2) {
            SPI.setClockDivider(SPI_CLOCK_DIV8);
          } else if (val == 3) {
            SPI.setClockDivider(SPI_CLOCK_DIV16);
          } else if (val == 4) {
            SPI.setClockDivider(SPI_CLOCK_DIV32);
          } else if (val == 5) {
            SPI.setClockDivider(SPI_CLOCK_DIV64);
          } else if (val == 6) {
            SPI.setClockDivider(SPI_CLOCK_DIV128);
          } else if (val == 7) {
            SPI.setClockDivider(SPI_CLOCK_DIV256);
          }
          break;

        case 0x23:  // SPI.setDataMode
          val = client.read();
          if (val == 0) {
            SPI.setDataMode(SPI_MODE0);
          } else if (val == 1) {
            SPI.setDataMode(SPI_MODE1);
          } else if (val == 2) {
            SPI.setDataMode(SPI_MODE2);
          } else if (val == 3) {
            SPI.setDataMode(SPI_MODE3);
          }
          break;

        case 0x24:  // SPI.transfer
          val = client.read();
          SPI.transfer(val);
          break;


        // Wire API
        case 0x30:  // Wire.begin
          address = client.read();
          if (address == 0) {
            Wire.begin();
          } else {
            Wire.begin(address);
          }
          break;
        case 0x31:  // Wire.requestFrom
          address = client.read();
          int quantity = client.read();
          stop = client.read();

          Wire.requestFrom(address, quantity, stop);
          break;
        case 0x32:  // Wire.beginTransmission
          address = client.read();
          Wire.beginTransmission(address);
          break;
        case 0x33:  // Wire.endTransmission
          stop = client.read();
          val = Wire.endTransmission(stop);
          client.write(0x33);
          client.write(val);
          break;
        case 0x34:  // Wire.write
          len = client.read();
          char wireData[len];
          for (i = 0; i< len; i++) {
            wireData[i] = client.read();
          }
          val = Wire.write(data, len);
          client.write(0x34);
          client.write(val);
          break;
        case 0x35:  // Wire.available
          val = Wire.available();
          client.write(0x35);
          client.write(val);
          break;
        case 0x36:  // Wire.read
          val = Wire.read();
          client.write(0x36);
          client.write(val);
          break;
      }
    }
  }
}