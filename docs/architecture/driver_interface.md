# Driver Interface

All motor profiles must implement the same conceptual interface.

```cpp
class MotorDriver {
public:
  bool begin();
  bool arm();
  void disarm();
  void setTargetPercent(int8_t targetPercent);
  void update();
  DriverStatus status() const;
};
```

- begin() must not emit motor output.
- disarm() must force zero output.
- setTargetPercent() must not directly write hardware.
- Hardware output occurs only in update() while state is Armed.
