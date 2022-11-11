
// Callback for Each Setpoint Update
BLYNK_WRITE(V4)
{
  setpoint = param.asLong();
  Serial.print("Temperature Set Point is: ");
  Serial.println(setpoint);
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
}