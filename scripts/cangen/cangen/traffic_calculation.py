class BusLoadCalculator:
 
    def __init__(self, can_speed = int):
        """"
        Initialize bus load calculator with the CAN Speed (bits per second)
        :param can_speed: The speed of the CAN Bus in bits per second
        
        """
        self.can_speed = can_speed 
    
    def frame_length(self, frame_data: tuple) -> float:
        total_message_length = 0
        """
        Calculate the total length of a single CAN frame in bits, this will include bit stuffing
        :param frame_data is tuple containing the bytes of data, and frames per second
        :return: Bus load as percentage
        """
        for messages in range(len(frame_data)):

            data_bytes, frequency_hz = frame_data 

        fixed_length = 44  #fixed length of CAN frame

        variable_length = data_bytes * 9.6 #accounted for bit stuffing

        total_frame_length = variable_length + fixed_length #add fixed and variable lengths

        total_message_length +=(total_frame_length* frequency_hz)

        return total_message_length
    
    def calculate_bus_load(self,messages: list[tuple])-> float:

        total_busload = 0.0

        for frame_data in messages:

            total_bits = self.frame_length(frame_data)  # Get bus load for the current message   

            total_busload += (total_bits/ (self.can_speed))*100 #bus load formula
           
        return round(total_busload,2)

# if __name__ == "__main__":   #TEST CASE USED
#     # CAN speed: 500 kbps
#     can_speed = 500000

#     messages = [
#         (8, 100),  # 8 bytes, 100Hz
#         (5, 50),
#         (2, 100),
#         (8, 50),  
#     ]

#     # Initialize the calculator
#     calculator = BusLoadCalculator(can_speed)

#     # Calculate total bus load
#     total_bus_load = calculator.calculate_bus_load(messages)

#     print(f"Total Bus Load: {total_bus_load}%")