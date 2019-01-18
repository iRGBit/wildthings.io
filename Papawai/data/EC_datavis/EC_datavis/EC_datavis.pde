// CSV file with 3 columns, no headers
// 1st column: MQTT channel (String) e.g. "moturoa/ec"
// 2nd column: sensor value (float) e.g. "123.4"
// 3rd column: timestamp in ms (int) e.g. "1234"


Table table;

float ecMax = 0;
float ecMin = 1024;
int time = 0;

void setup() {


  size(1280, 720);
  background (255);

  table = loadTable("15-12-18.CSV");

  int rowCount = table.getRowCount(); 

  for (TableRow row : table.rows()) {

    String channel = row.getString(0);
    float value = row.getFloat(1);
    int timestamp = row.getInt(2);
    switch(channel) {
    case "moturoa/ec":
      ecMax = max(ecMax, value);
      ecMin = min(ecMin, value);
      time = max(time, timestamp);
      break;
    default:             // Default executes if the case labels
      //println("None");   // don't match the switch parameter
      break;
    }
  }
  
  println("ecMax: ", ecMax, " & ecMin: ", ecMin, " , time: ", time);
  
  for (TableRow row : table.rows()) {

    String channel = row.getString(0);
    float value = row.getFloat(1);
    int timestamp = row.getInt(2);
    switch(channel) {
    case "moturoa/ec":
      float floatX = map(timestamp, 0, time, 0, width);
      float floatY = map(value, ecMin, ecMax, 0, height);
      println("x: ", floatX, "y: ", floatY);
      ellipse(floatX, floatY, 20, 20);
        break;
    default:             // Default executes if the case labels
      //println("None");   // don't match the switch parameter
      break;
    }
  }
}
