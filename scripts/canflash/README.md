# CAN Flash

## How to run the cantransfer.sh script

### Check if there is a connection with the pi

Type ``ping [ip address]`` in terminal, where ip address is the ip address of the pi.

### Running the script

Run the script by using the command ``sh cantransfer.sh 'bin_files' 'pi_username' 'host' 'destination_path``.

Where the inputs are 'bin_files' 'pi_username' 'host' 'destination_path':

- Bin files is the path to where the compiled firmware .bin files are located.
- Pi_username is the username of the raspberry pi.
- Host is the ip address of the pi.
- Destination path is the folder in the pi where the bin files will be transferred to.

If there is no specific path to sent the file to the directory ``/home/pi_username/`` will work.