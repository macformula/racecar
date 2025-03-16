#Check if all arguments have been passed
if [ "$#" -lt 3 ]; then
    echo "ERROR enter all arguments: sh cantransfer.sh 'bin_files' 'pi_username' 'IP_ADDRESS' 'destination_path'"
    exit 1
fi

BIN_FILE=$1
PI_USERNAME=$2
IP_ADDRESS=$3

if [ ! -z $4 ]; then
  echo "4th arguement is: $4"
  DESTINATION_PATH=$4
fi

# Check if the bin file exists
if [ ! -f "$BIN_FILE" ]; then
    echo "ERROR: File $BIN_FILE not found"
    exit 1
fi

# If destination path is set as an argument; it will be set as the destination path
# Otherwise it is set as the Pi's home directory
# If the destination path is a file, the path will be the PI's home directory
if [ ! -z $DESTINATION_PATH ]; then
  # Check if the path to the Pi exists 
  # Otherwise create create the path in the PI
  echo "Check if '$DESTINATION_PATH' path exists on the Pi..."

  ssh "$PI_USERNAME@$IP_ADDRESS" "test -d /home/$PI_USERNAME/$DESTINATION_PATH"

  if [ $? -eq 1 ]; then
    echo "Path doesn't exist, creating path in Pi..."
    ssh "$PI_USERNAME@$IP_ADDRESS" "mkdir -p '$DESTINATION_PATH'"
  else
    echo "Path exists in Pi"
  fi
fi

# Perform file transfer
if [ ! -z $DESTINATION_PATH ]; then
  echo "Transferring $BIN_FILE to $PI_USERNAME@$IP_ADDRESS:$DESTINATION_PATH."
  scp "$BIN_FILE" "$PI_USERNAME@$IP_ADDRESS:$DESTINATION_PATH"
else
# If destination path is empty
# The file will be placed in the the home directory
  echo "Transferring $BIN_FILE to $PI_USERNAME@$IP_ADDRESS:/home/$PI_USERNAME."
  scp "$BIN_FILE" "$PI_USERNAME@$IP_ADDRESS:/home/$PI_USERNAME"
fi