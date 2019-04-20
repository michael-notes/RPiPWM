#!/usr/bin/python3
# -*- coding: utf-8 -*-

import time
import RPi.GPIO as GPIO
import time

GPIO.setwarnings(False)

GPIOMode = GPIO.BOARD
GPIOType = GPIO.OUT
GPIOPin = 11
GPIOHz = 50
PulseDut = 50
PinRef = None
cmds = {
        "p":"pin", 
        "f":"frequency", 
        "d":"dut", 
        "e":"exit", 
        "r":"run",
        "pin":"pin", 
        "frequency":"frequency", 
        "dut":"dut", 
        "exit":"exit", 
        "run":"run"
}

GPIO.setmode(GPIOMode)
GPIO.setup(GPIOPin, GPIOType)

def help():
    print("""
cmd:
    1. p -> pin
    2. f -> frequency
    3. d -> dut
    4. e -> exit
    5. r -> run
    """)

def showSettings():

    global GPIOPin
    global GPIOHz
    global PulseDut

    print("GPIO Pin Settings: %d, Hz: %d, Dut: %d" % (GPIOPin, GPIOHz, PulseDut));


def getValue():
    try:
        value = int(input("value: "))
        if value < 0:
            return 0
    except:
        print("enter int format string")
        return -1

    return value

  
def PWM():

    global GPIOMode
    global GPIOPin
    global GPIOType
    global GPIOHz
    global PulseDut
    global cmds

    print("show defaut settings:")
    showSettings()
    print("------------------------------------")

    GPIO.setmode(GPIOMode)
    GPIO.setup(GPIOPin, GPIOType)
    PinRef = GPIO.PWM(GPIOPin, GPIOHz)
    PinRef.start(PulseDut)

    try:
        cmd = "-"
        while True:
            if cmd != "":
                print("cmd: p-pin, f-frequency, d-dut, e-exit, r-run")
            cmd = input("Enter: ")
            if cmd == "":
                continue

            if cmd != "r":
                if PinRef != None:
                    PinRef.stop()

            if cmd in cmds:
                for key in cmds:
                    if cmd == key:
                        print("cmd: " + cmds[key])
            else:
                print("cmd is not exist: " + cmd)
                continue

            if cmd == "p" or cmd == "pin":
                GPIOPin = getValue()

                GPIO.setmode(GPIOMode)
                GPIO.setup(GPIOPin, GPIOType)

                print("Pin is: " + str(GPIOPin))
            elif cmd == "f" or cmd == "frequency":
                GPIOHz = getValue()
                PinRef = GPIO.PWM(GPIOPin, GPIOHz)
                print("Hz is: " + str(GPIOHz))
            elif cmd == "d" or cmd == "dut":
                PulseDut = getValue()
                PinRef.ChangeDutyCycle(PulseDut)
                print("dut is: " + str(PulseDut))
            elif cmd == "e" or cmd == "exit":
                print("exit the program")
                exit()
            elif cmd == "r" or cmd == "run":
                PinRef.start(PulseDut)
                print("pwm settings has changed")
            else:
                help()

            print("------------------------------------")

    except KeyboardInterrupt:
        pass

    if PinRef != None:
        PinRef.stop()

    print("\r\n");

    GPIO.cleanup()

if __name__ == '__main__':
    help()
    PWM()

