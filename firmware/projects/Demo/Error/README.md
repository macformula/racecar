To generate the C++ CAN code, change into the `racecar/` and install cangen

```bash
pip install -r scripts/cangen/requirements.txt
```

then generate with

```bash
python scripts/cangen/main.py --project Demo/Error
```

## DBC

The DBC signals are all 1 bit booleans, so we can represent the existence / absence of 64 error types per message. I only included a handful.

The generated struct looks like

```c++
class TMS_ERROR : public shared::can::CanTxMsg {
public:
    bool temp_fault = 0;
    bool rtos_stack = 0;
    bool error_2 = 0;
    bool error_3 = 0;
    bool error_4 = 0;
    bool error_5 = 0;

    ...
```

Which tells you that you could indicate the presence of a temp-fault error by setting `instance.temp_fault=true`
