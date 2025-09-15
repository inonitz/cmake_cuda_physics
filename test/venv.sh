#!/bin/bash

# Credit to: 
# https://stackoverflow.com/questions/21620406/how-do-i-pause-my-shell-script-for-a-second-before-continuing
# https://stackoverflow.com/questions/41427500/creating-a-virtualenv-with-preinstalled-packages-as-in-requirements-txt
# https://stackoverflow.com/questions/66899666/how-to-install-from-requirements-txt

VIRTUAL_ENVIRONMENT_NAME=venvgeneric


pip install virtualenv
virtualenv ${VIRTUAL_ENVIRONMENT_NAME}
source ${VIRTUAL_ENVIRONMENT_NAME}/bin/activate
pip install -r requirements.txt


python3 test.py
read -p "Press any key to continue"