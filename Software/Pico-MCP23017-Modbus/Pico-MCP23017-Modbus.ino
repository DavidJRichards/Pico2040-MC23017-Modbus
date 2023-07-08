/******************************************************

djrm: 
test output: mbpoll -m rtu -b115200 -a10 -t0 -r1      /dev/ttyACM0 1 0 1 1 0 1 1 1 
test input:  mbpoll -m rtu -b115200 -a10 -t1 -1 -c 8  /dev/ttyACM0
report:      mbpoll -m rtu -b115200 -a10 -t0 -u       /dev/ttyACM0


Example sketch for the MCP23017/MCP23S17/MCP23018/
MCP23S18 library.

The sketch shows how to use the MCP23S18. The MCP23S18 is working
with SPI. All functions are the identical, except the constructors. 
The MCP23x18 ICs can deal with higher currents, but can only be used
as sink.

The wiring for this example can be found under the same name (png file). 

written by Wolfgang (Wolle) Ewald
https://wolles-elektronikkiste.de/en/port-expander-mcp23017-2

*******************************************************/

//#include <SPI.h>
#include <Wire.h>
#include <MCP23017.h>
#include <ModbusSerial.h>

#define MCP_ADDRESS 0x20 // (A2/A1/A0 = LOW)
// pins 26 & 27 need to use second i2c device Wire1 
#define MCP_SDA 26
#define MCP_SCL 27
//#define CS_PIN 7   // Chip Select Pin
/* A hardware reset is performed during init(). If you want to save a pin you can define a dummy 
 * reset pin >= 99 and connect the reset pin to HIGH. This will trigger a software reset instead 
 * of a hardware reset. 
 */
#define RESET_PIN 28 
//#define RESET_PIN 99 
//#define MCP_SPI_CTRL_BYTE 0x20 // Do not change

/* There are two ways to create your MCP23S18 object:
 * MCP23S18 myMCP = MCP23S18(CS_PIN, RESET_PIN, MCP_CTRL_BYTE);
 * MCP23S18 myMCP = MCP23S18(&SPI, CS_PIN, RESET_PIN, MCP_CTRL_BYTE);
 * The second option allows you to pass SPI objects, e.g. in order to
 * use two SPI interfaces on the ESP32.
 */
 
//MCP23S17 myMCP = MCP23S17(CS_PIN, RESET_PIN, MCP_SPI_CTRL_BYTE);
MCP23017 myMCP = MCP23017(&Wire1, MCP_ADDRESS, RESET_PIN);

const byte SlaveId = 10;
const int TxenPin = -1; // -1 disables the feature, change that if you are using an RS485 driver, this pin would be connected to the DE and /RE pins of the driver.
const unsigned long Baudrate = 115200;
ModbusSerial mb (Serial, SlaveId, TxenPin);

int wT = 500; // wT = waiting time
unsigned char reverse(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

void setup(){ 
  Serial.begin(Baudrate);
  mb.config (Baudrate);
  mb.setAdditionalServerData ("MCP23017"); // for Report Server ID function (0x11)

  while (!Serial && millis() < 5000);
  delay(100);
//  Serial.println("init...");
  Wire1.setSDA(MCP_SDA);
  Wire1.setSCL(MCP_SCL);
  Wire1.begin();

  if(!myMCP.Init()){
    Serial.println("Not connected!");
    while(1){} 
  }
//  Serial.println("Connected");
  myMCP.setPortMode(0xff, A);    
  myMCP.setPortMode(0x00, B);

  mb.addCoil (0);
  mb.addCoil (1);
  mb.addCoil (2);
  mb.addCoil (3);
  mb.addCoil (4);
  mb.addCoil (5);
  mb.addCoil (6);
  mb.addCoil (7);

  mb.addIsts(0);
  mb.addIsts(1);
  mb.addIsts(2);
  mb.addIsts(3);
  mb.addIsts(4);
  mb.addIsts(5);
  mb.addIsts(6);
  mb.addIsts(7);

}

void loop(){ 
//  int bits;
  mb.task();
//  bits=myMCP.getPort(B);
  // invert and bitreverse so switches and leds align on demo board
//  bits ^= 0xff;
//  bits=reverse(bits);
  myMCP.setPin(0,A,mb.Coil (7));
  myMCP.setPin(1,A,mb.Coil (6));
  myMCP.setPin(2,A,mb.Coil (5));
  myMCP.setPin(3,A,mb.Coil (4));
  myMCP.setPin(4,A,mb.Coil (3));
  myMCP.setPin(5,A,mb.Coil (2));
  myMCP.setPin(6,A,mb.Coil (1));
  myMCP.setPin(7,A,mb.Coil (0));

  mb.Ists(0, myMCP.getPin(0, B));
  mb.Ists(1, myMCP.getPin(1, B));
  mb.Ists(2, myMCP.getPin(2, B));
  mb.Ists(3, myMCP.getPin(3, B));
  mb.Ists(4, myMCP.getPin(4, B));
  mb.Ists(5, myMCP.getPin(5, B));
  mb.Ists(6, myMCP.getPin(6, B));
  mb.Ists(7, myMCP.getPin(7, B));

} 
