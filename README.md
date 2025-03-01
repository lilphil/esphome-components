# esphome-components

<h3 align="center">
ESPHome components
</h3>

## lolin i2c motor
A thin wrapper around the original lolin library

channel:
  * A
  * B
  * BOTH

status:
  * STOP
  * STANDBY
  * SHORT_BRAKE
  * CW
  * CCW

```yaml
external_components:
  - source: github://lilphil/esphome-components@main
    components: [ lolin_i2c_motor ]

lolin_i2c_motor:
  id: motor_controller

script:
  - id: cw
    mode: single
    then:
      - lambda: |-
          id(motor_controller).set_status("BOTH", "STOP");
          id(motor_controller).set_freq("BOTH", 1000);
          id(motor_controller).set_duty("BOTH", 100);
          id(motor_controller).set_status("BOTH", "CW");
  - id: ccw
    mode: single
    then:
      - lambda: |-
          id(motor_controller).set_status("BOTH", "STOP");
          id(motor_controller).set_freq("BOTH", 1000);
          id(motor_controller).set_duty("BOTH", 100);
          id(motor_controller).set_status("BOTH", "CCW");
  - id: stop
    mode: single
    then:
      - lambda: |-
          id(motor_controller).set_status("BOTH", "STOP");

```
