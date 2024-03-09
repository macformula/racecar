# Low Voltage Controller

Refer to macformula/lvcontroller

the following regex can be used to extract non-empty regions of USER code in cubemx generated files.

```regex
/\* USER CODE BEGIN ([\w]*) \*/\n[\S\s]*?[\w]+[\S\s]*?/\* USER CODE END \1 \*/
```
