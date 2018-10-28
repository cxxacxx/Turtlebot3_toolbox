/*******************************************************************************
* Copyright 2018 Xinxing Chen.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

/* Authors: Xinxing Chen */
/* Email: cxx@hust.edu.cn*/

#include <DynamixelWorkbench.h>

#if defined(__OPENCM904__)
  #define DEVICE_NAME "3" //Dynamixel on Serial3(USART3)  <-OpenCM 485EXP
#elif defined(__OPENCR__)
  #define DEVICE_NAME ""
#endif   

#define STRING_BUF_NUM 64
String cmd[STRING_BUF_NUM];
#define DXL_ID_1  1
#define DXL_ID_2  2

DynamixelWorkbench dxl_wb;
uint8_t get_id[16];
uint8_t scan_cnt = 0;
uint8_t ping_cnt = 0;

uint8_t dxl_id[2] = {DXL_ID_1, DXL_ID_2};
uint8_t dxl_cnt = 2;

bool isAvailableID(uint8_t id);
void split(String data, char separator, String* temp);
void printInst();

void setup() 
{
  Serial.begin(57600);
  //while(!Serial); // Open a Serial Monitor  

  printInst();

  //begin
  uint32_t baud = 1000000;
  if (dxl_wb.begin(DEVICE_NAME, baud))
    Serial.println("Succeed to begin(" + String(baud) + ")");
  else
    Serial.println("Failed to begin");

  //scan
  dxl_wb.scan(get_id, &scan_cnt, dxl_cnt);

      if (scan_cnt == 0)
        Serial.println("Can't find Dynamixel");
      else
      {
        Serial.println("Find " + String(scan_cnt) + " Dynamixels");
        for (int cnt = 0; cnt < scan_cnt; cnt++)
          Serial.println("ID : " + String(get_id[cnt]));
      }
    
}

void loop() 
{
  if (Serial.available())
  {
    String read_string = Serial.readStringUntil('\n');
    //Serial.println("[CMD] : " + String(read_string));

    read_string.trim();

    split(read_string, ' ', cmd);

    if (cmd[0] == "help")
    {
      printInst();
    }
    
    
    else if (cmd[0] == "ping")
    {
      if (cmd[1] == '\0')
        cmd[1] = String("1");

      get_id[ping_cnt] = cmd[1].toInt();
      uint16_t model_number = 0;

      if (dxl_wb.ping(get_id[ping_cnt], &model_number))
      {
        Serial.println("id : " + String(get_id[ping_cnt]) + " model_number : " + String(model_number));
        ping_cnt++;
      }
      else
        Serial.println("Failed to ping");
    }
    
      
      else if (cmd[0] == "enable")
      {
        uint8_t onoff = cmd[1].toInt();

        if (dxl_wb.itemWrite(1, "Torque_Enable", onoff)&&dxl_wb.itemWrite(2, "Torque_Enable", onoff))
        
          Serial.println("Succeed to enable");
        else
          Serial.println("Failed");
      }
      else if (cmd[0] == "set_pos")
      {
        for (int cnt = 0; cnt < dxl_cnt; cnt++)
        {
          dxl_wb.ping(dxl_id[cnt]);
          dxl_wb.jointMode(dxl_id[cnt]);
        }

        dxl_wb.initBulkWrite();
        dxl_wb.initBulkRead();

        dxl_wb.addBulkReadParam(dxl_id[0], "Present_Position");
        dxl_wb.addBulkReadParam(dxl_id[1], "Present_Position");
        uint16_t goal0 = cmd[1].toInt();
        uint16_t goal1 = cmd[2].toInt();
        if ((goal0<0)||(goal1<0))
          Serial.println("Wrong position set!");
        else{
          dxl_wb.addBulkWriteParam(dxl_id[0], "Goal_Position", goal0);
          dxl_wb.addBulkWriteParam(dxl_id[1], "Goal_Position", goal1);
          dxl_wb.bulkWrite();
          uint16_t present_position0;
          uint16_t present_position1;
        
          do
          {
            dxl_wb.setBulkRead();
    
            present_position0 = dxl_wb.bulkRead(dxl_id[0], "Present_Position");
            present_position1 = dxl_wb.bulkRead(dxl_id[1], "Present_Position");
          }while((abs(goal0 - present_position0)>20)||(abs(goal1-present_position1) > 20));
        
          Serial.println("Arrive at ("+String(present_position0)+","+String(present_position1)+")");
        }
      }
      else if (cmd[0] == "set_vel")
      {
        for (int cnt = 0; cnt < dxl_cnt; cnt++)
        {
          dxl_wb.ping(dxl_id[cnt]);
          dxl_wb.wheelMode(dxl_id[cnt]);
        }

        dxl_wb.initBulkWrite();

        
        uint16_t vel0 = cmd[1].toInt();
        uint16_t vel1 = cmd[2].toInt();
        
        dxl_wb.addBulkWriteParam(dxl_id[0], "Goal_Velocity", vel0);
        dxl_wb.addBulkWriteParam(dxl_id[1], "Goal_Velocity", vel1);
        dxl_wb.bulkWrite();
        Serial.println("Succeed to set vel!!");
      }
      else if (cmd[0] == "write")
      {
        uint8_t id = cmd[1].toInt();      
        int32_t value = cmd[3].toInt(); 

        if (dxl_wb.itemWrite(id, cmd[2].c_str(), value))
        {        
          Serial.println("Succeed to write command!!");
        }
        else
          Serial.println("Failed");
      }
      else if (cmd[0] == "read")
      {
        uint8_t id = cmd[1].toInt();
              
        int32_t value = dxl_wb.itemRead(id, cmd[2].c_str());

        Serial.println(String(value));
      }
      else if (cmd[0] == "reboot")
      {
        uint8_t id = cmd[1].toInt();

        if (dxl_wb.reboot(id))
          Serial.println("Succeed to reboot");
        else
          Serial.println("Failed to reboot");
      }
      else if (cmd[0] == "reset")
      {
        uint8_t id = cmd[1].toInt();

        if (dxl_wb.reset(id))
          Serial.println("Succeed to reset");
        else
          Serial.println("Failed to reset");
      }
      else 
      {
        Serial.println("Wrong command");
      }
    }
}

void split(String data, char separator, String* temp)
{
  int cnt = 0;
  int get_index = 0;

  String copy = data;
  
  while(true)
  {
    get_index = copy.indexOf(separator);

    if(-1 != get_index)
    {
      temp[cnt] = copy.substring(0, get_index);

      copy = copy.substring(get_index + 1);
    }
    else
    {
      temp[cnt] = copy.substring(0, copy.length());
      break;
    }
    ++cnt;
  }
}

bool isAvailableID(uint8_t id)
{
  for (int dxl_cnt = 0; dxl_cnt < (scan_cnt + ping_cnt); dxl_cnt++)
  {
    if (get_id[dxl_cnt] == id)
      return true;
  }

  return false;
}

void printInst(void)
{
  Serial.println("-------------------------------------");
  Serial.println("Set portHandler Before scan or ping");
  Serial.println("-------------------------------------");
  Serial.println("help");
  Serial.println("ping   (ID)");
  Serial.println("enable (1 or 0)");
  Serial.println("set_pos (pos1) (pos2)");
  Serial.println("set_vel (vel1) (vel2)");
  Serial.println("read (ID) (ADDRESS_NAME)");
  Serial.println("reboot (ID) ");
  Serial.println("reset  (ID) ");
  Serial.println("-------------------------------------");
}
