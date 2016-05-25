
String All_Name;
String Save_Name;
String Get_one_Name;

int loadNumberMember();
bool saveName();
bool loadConfig();



void setupJson() {
  Serial_println("Mounting FS...");

  if (!SPIFFS.begin()) {
    Serial_println("Failed to mount file system");
    return;
  }
  //loadNumberMember();

  saveName();

  //load_name_by_position("5");

  Add_New_Name("Hell");
  Add_New_Name_insert("7","YYYY");
  loadConfig();

}




bool loadConfig() {
  File1 configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial_println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial_println("Config file size is too large");
    return false;
  }
  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<1100> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial_println("Failed to parse config file");
    return false;
  }




  const char* Name = json["List_name"];
  All_Name = Name;

  const char* numName = json["member"];
  String numNametxt = numName;
  All_member_num = numNametxt.toInt();

  Serial_print("loadcon>>>");
  Serial_print("\tAll_Name=");
  Serial_print(All_Name);
  Serial_print("\tAll_member_num=");
  Serial_println(All_member_num);

  


  return true;
}





String load_name_by_position(String Find_position_is) {
  File1 configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial_println("Failed to open config file");
    //return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial_println("Config file size is too large");
    //return false;
  }
  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<1100> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial_println("Failed to parse config file");
    //return false;
  }


  Serial_print("loadBy>>>");

  const char* Name = json["List_name"];
  All_Name = Name;
  
  Serial_print("\tAll_Name=");
  Serial_println(All_Name);
  Serial_print("\tFind_position_is=");
  Serial_print(Find_position_is);
  
  int position_of_Text = All_Name.indexOf(Find_position_is);
  
//  Serial_print("\t indexOf=");
//  Serial_print(All_Name.indexOf(Find_position_is));
//  Serial_print("\t position_of_Text=");
//  Serial_print(position_of_Text);


  Serial_print("\t");
  if ( isDigit((All_Name.substring(position_of_Text + 1, position_of_Text + 2)).charAt(0)) ) {
    Serial_println("NUMBER");
    Get_one_Name = All_Name.substring(position_of_Text + 2, position_of_Text + 6);
  } else {
    Serial_println("TEXT");
    Get_one_Name = All_Name.substring(position_of_Text + 1, position_of_Text + 5);
  }

  
  Serial_print("\tGet_one_Name=");
  Serial_println(Get_one_Name);
  return Get_one_Name;
}


bool Add_New_Name(String addName) {
  StaticJsonBuffer<1100> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  JsonArray& Name = json.createNestedArray("Name");

  loadConfig();
  
  All_member_num++;

  All_Name+=All_member_num;
  All_Name+=addName;
  All_Name+="/";
  
  json["member"] = All_member_num;
  json["List_name"] = All_Name;

  File1 configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial_println("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
  return true;
}



bool Add_New_Name_insert(String address , String addName) {
  StaticJsonBuffer<1100> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  JsonArray& Name = json.createNestedArray("Name");

  loadConfig();
  
  int position_of_Text = All_Name.indexOf(address);
  
  if ( isDigit((All_Name.substring(position_of_Text + 1, position_of_Text + 2)).charAt(0)) ) {
    Serial_println("NUMBER");
    Get_one_Name = All_Name.substring(position_of_Text + 2, position_of_Text + 6);
  } else {
    Serial_println("TEXT");
    Get_one_Name = All_Name.substring(position_of_Text + 1, position_of_Text + 5);
  }
  
  All_Name.replace(Get_one_Name,addName);
  
  json["member"] = All_member_num;
  json["List_name"] = All_Name;

  File1 configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial_println("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
  return true;
}








int loadNumberMember() {

  File1 configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial_println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial_println("Config file size is too large");
    return false;
  }
  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<1100> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial_println("Failed to parse config file");
    return false;
  }


  const char* numName = json["member"];
  String numNametxt = numName;
  All_member_num = numNametxt.toInt();

  return true;
}


bool saveName() {
  StaticJsonBuffer<1100> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();


  json["member"] = 10;
  json["List_name"] = "1sudt/2sang/3sont/4sain/5onnu/6nong/7nap_/8name/9phun/10fon_/";

  File1 configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial_println("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
  return true;
}
