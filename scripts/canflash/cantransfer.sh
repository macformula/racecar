#Check if all arguments have been passed
if [ "$#" -lt 4 ]; then
    echo "ERROR enter all arguments: sh cantransfer.sh 'bin_files' 'pi_username' 'host' 'destination_path'"
    exit 1
fi

BIN_FILE=$1
PI_USERNAME=$2
HOST=$3
DESTINATION_PATH=$4

# Check if the bin folder exists
if [ ! -f "$BIN_FILE" ]; then
    echo "ERROR: File $BIN_FILE not found"
    exit 1
fi

# Perform file transfer
echo "Transferring $BIN_FILE to $PI_USERNAME@$HOST:$DESTINATION_PATH.."
scp "$BIN_FILE" "$PI_USERNAME@$HOST:$DESTINATION_PATH"