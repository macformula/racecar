#Check if all arguments have been passed
if [ "$#" -lt 3 ]; then
    echo "ERROR enter all arguments: sh cantransfer.sh 'bin_files' 'pi_username' 'IP_ADDRESS' 'destination_path'"
    exit 1
fi

BIN_FILE=$1
PI_USERNAME=$2
IP_ADDRESS=$3

# If destination path is set as an argument; it will be set as the destination path
# Otherwise it is set as the PI's home directory
# If the destination path is a file, the path will be the PI's home directory
if [ -d "3SM4" ]; then
  DESTINATION_PATH=$4
elif [ -f "3SM4" ]; then
  DESTINATION_PATH="/home/$PI_USERNAME"
else
  DESTINATION_PATH="/home/$PI_USERNAME"
fi

# Check if the bin file exists
if [ ! -f "$BIN_FILE" ]; then
    echo "ERROR: File $BIN_FILE not found"
    exit 1
    
# Check if the path to the PI exists 
# Otherwise create create the path in the PI
echo "Check if '$DESTINATION_PATH' exists on the PI..."

ssh "$PI_USERNAME@$IP_ADDRESS" "test -d '$DESTINATION_PATH'"
if [ $? -eq 0 ]; then
  echo "Path doesn't exist, creating path in PI..."
  ssh "$PI_USERNAME@$IP_ADDRESS" "mkdir -p '$DESTINATION_PATH'"
fi

# Perform file transfer
echo "Transferring $BIN_FILE to $PI_USERNAME@$IP_ADDRESS:$DESTINATION_PATH.."
scp "$BIN_FILE" "$PI_USERNAME@$IP_ADDRESS:$DESTINATION_PATH"