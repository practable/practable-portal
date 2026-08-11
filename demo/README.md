# Demo examples

## Duty

This is a simple firmware for an Arduino Nano 33 IOT to demonstrate the configurable interface.
It outputs a simple square wave signal via JSON messages.

### commands

There is only one command, which sets the duty cycle of the square wave. Send a newline-terminated JSON to set the duty cycle:

```
{"set":"duty_cycle","to":0.5}
```

The value: "duty_cycle" is a float clamped to 0.0–1.0.

Any other commands or malformed JSON are ignored

The output messages are sent every 100ms (10Hz), one JSON object per line:

```
{"timestamp":923423,"output":0}
```

timestamp: milliseconds since boot (integer)
output: synthesised square-wave value (0 or 1)

### UI

Note that in the configurable interface, you will be able to set up a button and a range to set the duty cycle, then record data to see the wave. 

