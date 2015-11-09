#!/bin/bash
while [ true ] ;
	do /usr/bin/JLinkGDBServer -IF SWD -device  STM32F746NG ;
	# kill off the troublesome gdb processes holding up port 2331
	lsof -i:2331 | awk '{print $2}' | tail -n 1 | xargs kill

sleep 1;
done;
