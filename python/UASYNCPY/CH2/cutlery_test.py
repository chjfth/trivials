#!/usr/bin/env python3

# Code from: [UASYNCPY] p16

import threading
from queue import Queue
 
class ThreadBot (threading.Thread): # thread subclass inherits start / join methods
    def __init__(self):
        super().__init__(target = self.manage_table) # objective function, defined below
        self.cutlery = Cutlery (knives = 0, forks = 0) # of each robot carrying tableware
        self.tasks = Queue () # robot receives the task is added to the task queue
 
    def manage_table(self): 
        while True: # robot accepts only three working
            task = self.tasks.get()
            if task == 'prepare table':
                kitchen.give(to=self.cutlery, knives=4, forks=4)
            elif task == 'clear table':
                self.cutlery.give(to=kitchen, knives=4, forks=4)
            elif task == 'shutdown':
                return
# Cutlery
from attr import attrs, attrib 
	# open source library does not affect the thread or coroutine, make it easier to initialize instance attributes
	# pip install attrs
 
@attrs
class Cutlery:
    knives = attrib(default=0)
    forks = attrib(default=0)
 
    def give (self, to: 'Cutlery', knives = 0, forks = 0): # is used to interact with other examples
        self.change(-knives, -forks)
        to.change(knives, forks)
 
    def change(self, knives, forks):
        self.knives += knives
        self.forks += forks
 
kitchen = Cutlery(knives=100, forks=100)
bots = [ThreadBot () for i in range (10)] # 10 threads created robot
 
import sys
 
for bot in bots:
    for i in range (int(sys.argv[1])): # get the number of the table from the command line, and then arrange all the tables for each robot task
        bot.tasks.put('prepare table')
        bot.tasks.put('clear table')
    bot.tasks.put('shutdown')
 
print(f'Kitchen inventory before service: {kitchen}')

for bot in bots: 
	bot.start()
	
for bot in bots: 
	bot.join()

print(f'Kitchen inventory after service: {kitchen}')
