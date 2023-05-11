float volt[10];
String gonder;


void setup() {
  Serial3.begin(9600);
  Serial.begin(115200);
}

void loop() {
  volt[0] = analogRead(A0) * (5.0/1023.0);
  volt[1] = analogRead(A1) * (5.0/1023.0);
  volt[2] = analogRead(A2) * (5.0/1023.0);
  volt[3] = analogRead(A3) * (5.0/1023.0);
  volt[4] = analogRead(A4) * (5.0/1023.0);
  volt[5] = analogRead(A5) * (5.0/1023.0);
  volt[6] = analogRead(A6) * (5.0/1023.0);
  volt[7] = analogRead(A7) * (5.0/1023.0);
  volt[8] = analogRead(A8) * (5.0/1023.0);
  volt[9] = analogRead(A9) * (5.0/1023.0);

  

  for(int m = 0; m < 10; m++){
    gonder.concat(String(volt[m]) + ",");
  }
  Serial3.println(gonder);
}
