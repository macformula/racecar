
# git clone https://github.com/macformula/racecar.git

# cd racecar

# git pull

# cd scripts/canflash

python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt

python3 main.py
