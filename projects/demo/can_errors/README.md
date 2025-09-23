# CANErrors
This is the CAN Errors demo project. It defines a global system for the vehicle to send/receive errors through different subsystems. It defines:
- A 64 bit CAN error signal. Each bit represents a different error, and will be sent periodically for other systems to know if an error has occurred
- The `ErrorHandler` class, an interface to set/send different errors on the CAN signal
- Enum `Error` that defines all 64 possible errors to send through the CAN signal

## How to use `ErrorHandler`

### Defining an instance of ErrorHandler:
```
ErrorHandler error_handler{};
```

### Setting errors with ErrorHandler:
- `SetError` takes an error from the `Error` enum and sets it
```
error_handler.SetError(Error0);
error_handler.SetError(Error15);
error_handler.SetError(Error48);
```

### Sending errors with ErrorHandler:
- `SendError` takes a CAN bus and sends the errors through that bus. It resets the errors to send the next set of errors
```
error_handler.SendError(error_can_bus);
```

## Running the Project with PlatformIO

Select the CLI environment then click Upload.