h
loadbin %filename% 0x10000000
w4 0x40043100 0x10000000
w4 0xE000ED08 0x10000000
r
g
q
