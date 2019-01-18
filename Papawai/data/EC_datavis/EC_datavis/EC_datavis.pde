// CSV file with 3 columns, no headers
// 1st column: MQTT channel (String) e.g. "moturoa/ec"
// 2nd column: sensor value (float) e.g. "123.4"
// 3rd column: timestamp in ms (int) e.g. "1234"


Table table;

float ecMax, ec2Max = 0.0;
float ecMin, ec2Min = 1024.0;
float tempMax = -100.0;
float tempMin = 100.0;

int time = 0;
int count1 = 0;
int count2 = 0;

void setup() {

  noStroke();
  size(1280, 720);
  background (255);

  table = loadTable("15-12-18.CSV");

  int rowCount = table.getRowCount(); 

  for (TableRow row : table.rows()) {

    String channel = row.getString(0);
    float value = row.getFloat(1);
    int timestamp = row.getInt(2);

    time = max(time, timestamp);

    switch(channel) {
    case "moturoa/ec":
      ecMax = max(ecMax, value);
      if (ecMin > 30) {
        ecMin = min(ecMin, value);
      }
      break;
    case "moturoa/ecrua":
      ec2Max = max(ec2Max, value);
      if (ec2Min > 30) {
        ec2Min = min(ec2Min, value);
      }
      break;
    case "moturoa/watertemp":
      tempMax = max(tempMax, value);
      tempMin = min(tempMin, value);

      break;
    default:             // Default executes if the case labels
      //println("None");   // don't match the switch parameter
      break;
    }
  }

  println("ecMax: ", ecMax, " & ecMin: ", ecMin, " , time: ", time, " tempMax: ", tempMax, ", tempMin: ", tempMin);

  for (TableRow row : table.rows()) {

    String channel = row.getString(0);
    float value = row.getFloat(1);
    int timestamp = row.getInt(2);
    switch(channel) {
    case "moturoa/ec":
      float floatX = map(timestamp, 0, time, 0, width);
      float floatY = map(value, min(ecMin, ec2Min), max(ecMax, ec2Max), height-100, 0);
      //println("x: ", floatX, "y: ", floatY);
      fill(255, 214, 8, 155);
      ellipse(floatX, floatY, 20, 20);
      fill(0);
      if (count2%20==0) {
        text(value, floatX, floatY-20);
      }
      count1++;
      break;
    case "moturoa/ecrua":
      float floatX2 = map(timestamp, 0, time, width, 0);
      float floatY2 = map(value, min(ecMin, ec2Min), max(ecMax, ec2Max), height-100, 0);
      //println("x: ", floatX2, "y: ", floatY2);
      fill(255, 0, 111, 155);
      ellipse(floatX2, floatY2, 20, 20);
      fill(0);
      if (count2%20==0) {
        text(value, floatX2, floatY2-20);
      }
      count2++;
      break;
    case "moturoa/watertemp":
      float x = map(timestamp, 0, time, width, 0);
      float y = map(value, tempMin, tempMax, height-100, 100);
      fill(0);
      text(value, x, y-10);
      ellipse(x, y, 10, 10);

      break;
    default:             // Default executes if the case labels
      //println("None");   // don't match the switch parameter
      break;
    }
  }
}
