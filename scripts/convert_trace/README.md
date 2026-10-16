# Jsonl to MDF

Convert a [JSON lines](https://github.com/macformula/hil/blob/main/canlink/jsonl.go) log file from our [CAN tracer](https://github.com/macformula/hil/blob/main/canlink/tracer.go) to an MDF file.

For developers to convert JSON lines log format to MDF for integration with [`asammdf`](https://asammdf.readthedocs.io/en/latest/). With this format, visualization tools such as [Asammdf GUI](https://canlogger.csselectronics.com/canedge-getting-started/ce2/log-file-tools/asammdf-gui/) can be used to view and debug traces.

# Usage

It is recommended to use uv python package manager.

```bash
python ./jsonl_to_mdf.py ./trace.jsonl
```
