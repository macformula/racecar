# CANErrors
This is the CAN Errors demo project. It defines a global system for the vehicle to send/receive errors through different subsystems. It defines:
- A 64 bit CAN error signal. Each bit represents a different error, and will be sent periodically for other systems to know if an error has occurred
- The `ErrorHandler` class, an interface to set/send different errors on the CAN signal
- Enum `Error` that defines all 64 possible errors to send through the CAN signal

<br>

## How to use `ErrorHandler`

### Defining an instance of ErrorHandler:
```
ErrorHandler error_handler{};
```

### Setting errors with ErrorHandler:
- `setError` takes an error from the `Error` enum and sets it
```
error_handler.setError(Error0);
error_handler.setError(Error15);
error_handler.setError(Error48);
```

### Sending errors with ErrorHandler:
- `sendError` takes a CAN bus and sends the errors through that bus. It resets the errors to send the next set of errors
```
error_handler.sendError(error_can_bus);
```

<br>

## Building the Project
- To build the project, run the following in the firmware directory "make PROJECT=Demo/CANErrors PLATFORM=cli"
   - This should create a build file with no errors!
   - This will also generate the dbc files automatically, but if you want to generate them seperately, run "cangen projects/Demo/CANErrors"
- To execute the new build, run "./build/Demo/CANErrors/cli/main.exe"