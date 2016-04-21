#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#define I2C_ADR 	0x5c
#define I2C_DEVICE 	"/dev/i2c-1"


int i2c_init(void)
{
    int file;
    char filename[40];
    int addr = I2C_ADR;    // The I2C address

    sprintf(filename,I2C_DEVICE);
    if ((file = open(filename,O_RDWR)) < 0) {
        printf("Failed to open the bus.");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        exit(1);
    }

    if (ioctl(file,I2C_SLAVE,addr) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        exit(1);
    }
    return file;
}

void i2c_read_reg(int file, char reg, char *buf, char length)
{
	char adr,i;
	adr=reg;
	if (write(file,&adr,1) != 1) 
	{
        	/* ERROR HANDLING: i2c transaction failed */
        	printf("Failed to write to the i2c bus.\n");
        }
        if (read(file,buf,length) != length) 
        {
           	/* ERROR HANDLING: i2c transaction failed */
          	printf("Failed to read from the i2c bus.\n");
        }
}

void i2c_write_reg(int file, char reg, char *buf, char length)
{
	char adr,i;
	adr=reg;
	if (write(file,&adr,1) != 1) 
	{
        	/* ERROR HANDLING: i2c transaction failed */
        	printf("Failed to write to the i2c bus.\n");
        }
        if (write(file,buf,length) != length) 
        {
           	/* ERROR HANDLING: i2c transaction failed */
          	printf("Failed to write to the i2c bus.\n");
        }
}


void print_at(char x, char y)
{
    printf("\033[%d;%dH",y,x);
}


int main(void) 
{
#define raw_x 20
#define raw_y 1
#define test_x 21
#define test_y 10
#define test_w 32
#define test_h 12
	
	int i2c;
	uint8_t touch_num,tmp[4],raw[110],i,j,dx1,dy1,dx2,dy2;
	int16_t X1,X2,Y1,Y2;

	i2c=i2c_init();
	printf("\033[2J");
	while(1)
	{
		i2c_read_reg(i2c,109,&touch_num,1);
		printf("\033[H");
		printf("\033[37;1mtouch_num=\033[32;1m%d\033[0m\n",touch_num);
		X1=X2=Y1=Y2=-1;
		dx1=dx2=dy1=dy2=0;
		switch(touch_num)
		{
		    case 2:	i2c_read_reg(i2c,66,tmp,2);
				i2c_read_reg(i2c,70,tmp+2,2);
				i2c_read_reg(i2c,0x50,&dx2,1);
				i2c_read_reg(i2c,0x52,&dy2,1);
				X2=((tmp[2] & 0x0F)<<8) | tmp[0];
				Y2=((tmp[3] & 0x0F)<<8) | tmp[1];
		    case 1:	i2c_read_reg(i2c,64,tmp,2);
				i2c_read_reg(i2c,68,tmp+2,2);
				i2c_read_reg(i2c,0x4f,&dx1,1);
				i2c_read_reg(i2c,0x51,&dy1,1);
				X1=((tmp[2] & 0x0F)<<8) | tmp[0];
				Y1=((tmp[3] & 0x0F)<<8) | tmp[1];
		}
		printf("\n");
		printf("\033[35;1mTouch point 1:\n");
		printf("\033[33;0mX=");
		if (X1>=0) printf("\033[32;1m %d          \n",X1); else printf("\033[31;1m n/a        \n");
		printf("\033[33;0mY=");
		if (Y1>=0) printf("\033[32;1m %d          \n",Y1); else printf("\033[31;1m n/a        \n");
		printf("\033[33;0mdX=");
		printf("\033[32;1m %d   \n",dx1);
		printf("\033[33;0mdY=");
		printf("\033[32;1m %d   \n",dy1);

		printf("\n");
		printf("\033[35;1mTouch point 2:\n");
		printf("\033[33;0mX=");
		if (X2>=0) printf("\033[32;1m %d          \n",X2); else printf("\033[31;1m n/a        \n");
		printf("\033[33;0mY=");
		if (Y2>=0) printf("\033[32;1m %d          \n",Y2); else printf("\033[31;1m n/a        \n");
		printf("\033[33;0mdX=");
		printf("\033[32;1m %d   \n",dx2);
		printf("\033[33;0mdY=");
		printf("\033[32;1m %d   \n",dy2);

		i2c_read_reg(i2c,0,raw,110); //read all
		print_at(raw_x,raw_y);
		printf("\033[37;1m");
		for(i=0;i<16;i++)
		{
		    print_at(raw_x+2+i*2,raw_y);
		    printf("\033[37;42;%dm%02X",(i%2)? 1:22,i);
		    if(i<7)
		    {
			print_at(raw_x,raw_y+i+1);
			printf("\033[37;42;%dm%02X",(i%2)? 1:22,i*16);
		    }
		}
		for(i=0;i<110;i++)
		{
		    print_at(raw_x+2+(i%16)*2,raw_y+1+(i/16));
		    switch(i)
		    {	case 64:
			case 65:
			case 68:
			case 69:printf("\033[37;45;%dm%02X",(i%2)? 1:22,raw[i]);
				break;
			case 66:
			case 67:
			case 70:
			case 71:printf("\033[37;42;%dm%02X",(i%2)? 1:22,raw[i]);
				break;
			
			case 0x4f:
			case 0x50:
			case 0x51:	    
			case 0x52: printf("\033[37;43;%dm%02X",(i%2)? 1:22,raw[i]);
				break;
			case 109:printf("\033[37;46;%dm%02X",(i%2)? 1:22,raw[i]);
				break;

			default:
			printf("\033[37;44;%dm%02X",(i%2)? 1:22,raw[i]);
		    }
		}
		printf("\033[0m\033[44m");
		for(i=0;i<test_h;i++)
		{
		    print_at(test_x,test_y+i);
		    for(j=0;j<test_w;j++) printf(" ");
		}
				    printf("\033[33m");
		switch(touch_num)
		{
		    case 2:
		    print_at(test_x+(int)((double)X2/800.0*test_w),test_y+(int)((double)Y2/480.0*test_h));
		    printf("2");
		    case 1:
		    print_at(test_x+(int)((double)X1/800.0*test_w),test_y+(int)((double)Y1/480.0*test_h));
		    printf("1");
		}
		print_at(1,2);
		printf("\033[0m");
		usleep(10000);
	}
	return 0;
}
