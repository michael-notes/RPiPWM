all:
	gcc RPiPWMH.c -o RPiPWM -lwiringPi
clean:
	rm RPiPWM
test:
	./RPiPWM
