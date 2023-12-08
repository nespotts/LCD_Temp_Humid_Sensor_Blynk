void syncPins() {
  if (!adjust_setpoint_flag) {
    Serial.println("Syncing virtual pins");
    Blynk.syncVirtual(V4, V12, V14);
  }
}


void recordSetpoint() {
  if (!adjust_setpoint_flag) {
    Serial.println("Recording Setpoint");
    Blynk.virtualWrite(V4, setpoint);
  }
}


// Callback for Each Setpoint Update
BLYNK_WRITE(V4)
{
  if (setpoint != param.asLong()) {
    setpoint = param.asLong();
    Serial.print("Temperature Set Point is: ");
    Serial.println(setpoint);
  }
}

BLYNK_WRITE(V12)
{
  firewood_needed_time = param.asInt();
}

BLYNK_WRITE(V14)
{
  firewood_needed = param.asInt();
  Serial.print("Firewood Status set to: ");
  Serial.println(firewood_needed);
  t2 = millis();
}