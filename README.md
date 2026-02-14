# esphome-components

<h3 align="center">
ESPHome components
</h3>

## i2c_motor_driver
Unified I2C motor driver component. Supports **lolin_v2** and **wemos_v1** via config.

**driver:** `lolin_v2` | `wemos_v1`
**channel:** A | B | BOTH
**status:** STOP | STANDBY | SHORT_BRAKE | CW | CCW

Optional: `i2c_address` (default 0x30), `stby_pin` (GPIO for wemos_v1 standby).

```yaml
external_components:
  - source: github://lilphil/esphome-components@main
    components: [ i2c_motor_driver ]

i2c_motor_driver:
  id: motor_controller
  driver: lolin_v2

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
