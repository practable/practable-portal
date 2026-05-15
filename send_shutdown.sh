#!/bin/bash

# Set up GPIO 22
GPIO_PIN_17=529      #  gpio17
GPIO_PIN_27=539
GPIO_PIN_22=534  ## why have they changed these?

GPIO_PIN=$GPIO_PIN_22

BASE_GPIO_PATH=/sys/class/gpio

ON="1"
OFF="0"


#Utility function to export pins if not already exported
exportPin()
{
  if [ ! -e $BASE_GPIO_PATH/gpio$1 ]; then
    echo "$1" | sudo tee $BASE_GPIO_PATH/export >/dev/null
  fi
}

#utility to set pin as output
setOutput()
{
  echo "out" | sudo tee $BASE_GPIO_PATH/gpio$1/direction >/dev/null
}

#Utility to change state of the output
setGPIOstate()
{
  echo $2 | sudo tee  $BASE_GPIO_PATH/gpio$1/value >/dev/null
}

# Utility function to turn all lights off
allGPIOlow()
{
  setGPIOstate $GPIO_PIN $OFF
}

# Ctrl-C handler for clean shutdown
shutdown()
{
  allGPIOlow
  exit 0
}


# Set up Handler
trap shutdown SIGINT
#Export pins for use
exportPin $GPIO_PIN
# Set pins as outputs
setOutput $GPIO_PIN

echo "setting GPIO"$GPIO_PIN" HIGH"
setGPIOstate $GPIO_PIN $ON
echo "Holding for 4 seconds"
sleep 4

setGPIOstate $GPIO_PIN $OFF
echo "Shutdown Sent"

