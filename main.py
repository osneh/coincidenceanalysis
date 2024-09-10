import os
import logging

from logging.handlers import RotatingFileHandler

CWD = os.path.sep.join(os.path.abspath(__file__).split(os.path.sep)[:-1])
Data = os.path.sep.join([CWD,'data'])
Src = os.path.sep.join([CWD,'src'])
Logs = os.path.sep.join([CWD,'logs'])
LogFile = os.path.sep.join([Logs,'App.log'])

logging.basicConfig(format = "%(asctime)s %(levelname)s :: %(message)s", level=logging.DEBUG)
logger = logging.getLogger('Analysis')
handler = RotatingFileHandler(LogFile, maxBytes=1000000, backupCount=100, encoding='utf-8',delay=0)
handler.setLevel(logging.INFO)
handler.setFormatter(logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s'))
logger.addHandler(handler)

##print(CWD)
##print(Data)
##print(Src)

for folder in [Data, Src]:
    if os.path.isdir(folder):
        logger.info(f"Folder is good: {folder}")
    else :
        print("Missing folder:, aborting execution!")
