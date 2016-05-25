bool Staff[22] ;/*= {false,false,false,false,false,false,false,false,false,false
,false,false,false,false,false,false,false,false,false,false
,false,false,false,false,false,false,false,false,false,false
,false,false,false,false,false,false,false,false,false,false
,false,false,false,false,false,false,false,false,false,false}; 
*/
int ID_Card;

MqttConnector::prepare_data_hook_t on_prepare_data =
[&](JsonObject * root) -> void {


    JsonObject& data = (*root)["d"];
    //JsonObject& info = (*root)["info"];

    
//    String Send_val="";
//    for(int i=0 ;i<50;i++){
//      if(Staff[i]){
//        Send_val += "1";
//      }else{
//        Send_val += "0";
//      }
//    }
//    Serial.println(Send_val);
//    data["Staff"] = Send_val;


    for(int i=0 ;i<=21;i++){
      String j="St_";
      j+=i;
      if(Staff[i]){
        data[j] = "1";
      }else{
        data[j] ="0";
      }
    }


    
//    data["Staff_0"] = Staff[0];
//    data["Staff_1"] = Staff[1];
//    data["Staff_2"] = Staff[2];
//    data["Staff_"] = Staff[];
};
