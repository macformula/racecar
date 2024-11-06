#Check if all arguements have been passed
if [ "$#" -lt 4 ]; then
    echo "ERROR enter all arguements: sh cantransfer.sh 'bin_files' 'pi_username' 'host' 'destination_path'"
    exit 1
fi

BIN_FILES=$1
PI_USERNAME=$2
HOST=$3
DESTINATION_PATH=$4

# Check if the bin folder exists
if [ ! -d "$BIN_FILES" ]; then
    echo "Error: File $BIN_FILES not found."
    exit 1
fi

# Perform file transfer
echo "Transferring $BIN_FILES files to $PI_USERNAME@$HOST:$DESTINATION_PATH.."
scp -r "$BIN_FILES/*" "$PI_USERNAME@$HOST:$DESTINATION_PATH"