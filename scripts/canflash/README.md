# CAN Flash

## How to run the cantransfer.sh script

### Check if there is a connection with the pi

Type ``ping [ip address]`` in terminal, where ip address is the ip address of the pi.

### Running the script

Run the script by using the command ``sh cantransfer.sh 'bin_files' 'pi_username' 'host' 'destination_path``.

Where the inputs are 'bin files' 'pi username' 'ip address' 'destination path':

- Bin files is the path to where the compiled firmware .bin files are located.
- Pi_username is the username of the raspberry pi.
- ip_address is the ip address of the pi.
- Destination path is the folder in the pi where the bin files will be transferred to.

If no directory is given, the bin file will be sent to the home directory of the pi: ``/home/pi_username/``.