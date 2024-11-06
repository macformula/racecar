# CAN Flash

# How to run the cantransfer.sh script

## Check if there is a connection with the pi

type "ping [ip address]" in git bash
where ip address is the ip address of the pi

## Running the script

run the script by using the command "sh cantransfer.sh 'bin_files' 'pi_username' 'host' 'destination_path'"

where the inputs are 'bin_files' 'pi_username' 'host' 'destination_path'

- bin files is the path to where the compiled firmware .bin files are located
- pi_username is the username of the raspberry pi
- host is the ip address of the pi
- destination path is the folder in the pi where the bin files will be transferred to

if there is no specific path to sent the file to the directory /home/pi_username/ will work