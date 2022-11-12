
// Callback for Each Setpoint Update
BLYNK_WRITE(V16)
{
  setpoint = param.asLong();
  Serial.print("Temperature Set Point is: ");
  Serial.println(setpoint);
}

BLYNK_WRITE(V17)
{
  firewood_needed_time = param.asInt();
}

BLYNK_WRITE(V18)
{
  firewood_needed = param.asInt();
  Serial.print("Firewood Status set to: ");
  Serial.println(firewood_needed);
  t2 = millis();
}