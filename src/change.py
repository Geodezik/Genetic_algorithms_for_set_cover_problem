import os

with open("EncodingSotnezovBCGA.cpp", 'r') as file:
    s = file.read().replace("EncSotnezovBCGA", "GENCODE")
with open("EncodingSotnezovBCGA.cpp", 'w') as file:
    file.write(s)