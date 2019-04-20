#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define PINNUM          18
#define BUFFER_SIZE     32


char *buffer            = NULL;
size_t bufsize          = BUFFER_SIZE;
size_t characters       = 0;

int crystal             = 19200000;
#define RANGE_MAX       4096
#define RANGE_MIN       0
int rangeReg            = 1000;
#define DIV_MAX         4095
#define DIV_MIN         2
int divReg              = 1920;
int dutReg              = 500;
#define CRYSTAL         19200000
#define RANGE_REG       1000
#define DIV_REG         1920
int freq                = CRYSTAL / DIV_REG / RANGE_REG;
#define PIN_MAX         40
#define PIN_MIN         40
int pinNum              = PINNUM;


typedef void (*Callback)(int);
void call_pin(int input);
void call_frequency(int input);
void call_dut(int input);
void call_range(int input);
void call_exit(int input);
void call_help(int input);

struct CMD {
    char* s;
    char* l;
    char c;
    Callback call;
};
struct CMD *cmd = NULL;
struct CMD cmds[] = {
    {"p", "pin", sizeof("pin"),  call_pin},
    {"f", "frequency", sizeof("frequency"), call_frequency},
    {"d", "dut", sizeof("dut"), call_dut},
    {"r", "range", sizeof("range"), call_range},
    {"e", "exit", sizeof("exit"), call_exit},
    // {"h", "help", sizeof("help"), call_help},
    NULL
};


int getInt() 
{
    int ret = 0;

    bzero(buffer, BUFFER_SIZE);

    characters = getline(&buffer,&bufsize,stdin);
    buffer[characters - 1] = 0;
    if (characters <= 1)
        return -1;
    else {
        size_t span = strspn(buffer, "0123456789");
        if(buffer[span] == '\0') {
            return atoi(buffer);
        } else {
            return -2;
        }
    }
}

void currentSettings()
{
    printf("info:\r\n"
           "    pin number: %d, frequency: %d\r\n"
           "    range: %d, dut:%d, div: %d\r\n"
           "-----------------------------------------\r\n",
           pinNum, freq, rangeReg, dutReg, divReg
          );
}

int run()
{
    if (wiringPiSetupGpio() == -1) {
        printf("error: can not init wiringPi lib\r\n");
        exit (1) ;
    }
    
    pinMode(pinNum, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);             
    pwmSetClock(divReg);
    pwmSetRange (rangeReg) ;
    pwmWrite (pinNum, dutReg);

    currentSettings();
}

void call_pin(int input) 
{
    if (input >= 0 && input < 40) {
        pinNum = input;
    } else 
        printf("pin is %d ~ %d\r\n", PIN_MIN, PIN_MAX);
}

void call_frequency(int input) 
{
    int temp = 0;

    if (input > 0 && crystal > 0 && rangeReg > 0){
        temp = crystal / rangeReg / input;
        if (temp >= DIV_MIN && temp <= DIV_MAX) {
            freq = input;
            divReg = temp;
            run();
        }
    } else 
        printf("frequency is %d ~ %d\r\n", crystal / rangeReg / DIV_MAX, crystal / rangeReg / DIV_MIN);
}

void call_dut(int input) 
{
    if (input >= RANGE_MIN && input < rangeReg) {
        dutReg = input;
        run();
    } else 
        printf("dut is 0 ~ %d\r\n", rangeReg);
}

void call_range(int input)
{
    if (input >= RANGE_MIN && input <=  RANGE_MAX) {
        if (dutReg >= input)
            dutReg = input / 2;

        rangeReg = input;
        freq = crystal / rangeReg / divReg;
        run();
    } else
        printf("range is %d ~ %d\r\n", RANGE_MIN, RANGE_MAX);
}

void call_exit(int input)
{
    printf("^_^ bye bye\r\n");
    exit(0) ;
}

void call_help(int input)
{
    cmd = cmds;
    printf("help: ");
    do {
        printf("%s->%s ", cmd->s, cmd->l);
        cmd++;
    } while (*(int*)(cmd));
    printf("\r\n");
}

void call_default(int input)
{
    run();
}


int main(int argc, char *argv[])
{
    buffer = (char *)malloc(bufsize * sizeof(char));

    // currentSettings();
    call_default(0);
    call_help(0);

    while (1) {

        bzero(buffer, BUFFER_SIZE);
        printf("$ : ");

        characters = getline(&buffer, &bufsize, stdin);
        if (characters <= 1) 
            continue;

        buffer[characters - 1] = 0;         // replace '\n' to '\0' string end
        characters -= 1;

        cmd = cmds;
        do {
            if (characters == 1) {
                if (strncmp(buffer, cmd->s, 1) == 0) 
                    break;
            } else {
                if (strncmp(buffer, cmd->l, cmd->c) == 0)
                    break;
            }

            cmd++;
        } while (*(int*)(cmd));

        if (*(int*)(cmd) && cmd->call) {
            if (strncmp(cmd->l, "exit", cmd->c) == 0)
                cmd->call(0);
            else {
                printf("$ < input value: ");
                cmd->call(getInt());
            }
        } else {
            printf("Warning: plz check your cmd\r\n");
        }

        call_help(0);
    }

    return 0;
}
