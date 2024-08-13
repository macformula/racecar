# Demo Project RTOS

Three FreeRTOS tasks are created

- `TestTask` - this task polls a button at 1kHz, when pressed a semaphore `sem_test` is signaled
- `LedTask` - this task tries to acquire a semaphore `sem_test`. When the button is pressed, the semaphore is signaled and `StartLedTask` toggles the red LED
- `MessageSendTask` - this task starts a one-shot software timer `timer_test`. The task then starts polling a FIFO `message_queue_test` for new messages to copy to its local buffer. Once the timer callback `MessageTimerCallback` fires after 3 seconds, the timer writes the contents of a string buffer to the FIFO. Once `MessageSendTask` sees that the entire string has been copied, it checks to make sure the received string is the same as the original string. If true, the blue LED is set.
