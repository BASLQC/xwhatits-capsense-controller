/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  Bit bang TWI master driver.
 *
 *      This file contains the function prototypes and enumerator definitions
 *      for various configuration parameters for the AVR TWI master driver.
 *
 *      The driver is not intended for size and/or speed critical code, since
 *      most functions are just a few lines of code, and the function call
 *      overhead would decrease code performance. The driver is intended for
 *      rapid prototyping and documentation purposes for getting started with
 *      the AVR TWI master.
 *
 *      For size and/or speed critical code, it is recommended to copy the
 *      function contents directly into your application instead of making
 *      a function call.
 *
 * 
 * $Date: 2012-06-01 13:03:43 $  \n
 *
 * Copyright (c) 2012, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  Author: gary.grewal
 *****************************************************************************/
#include "twi.h"

/*! \brief initialize twi master mode
*/ 
void twi_init()
{
    DDR_SCL |= (1 << SCL);
    DDR_SDA |= (1 << SDA);

    write_sda(1);
    write_scl(1);

} 

/*! \brief disables twi master mode
*/
void twi_disable()
{
    DDR_SCL &= ~(1 << SCL);
    DDR_SDA &= ~(1 << SDA);

}

/*! \brief Sends start condition
*/
char twi_start_cond(void)
{
    write_sda(0);
    __builtin_avr_delay_cycles(DELAY);

    write_scl(0);	
    __builtin_avr_delay_cycles(DELAY);
    return 1;

}

/*! \brief Sends slave address
*/
char send_slave_address(unsigned char read)
{
    return i2c_write_byte(SLAVE_ADDRESS | read );
} 

/*! \brief Writes data from buffer.
  \param indata Pointer to data buffer
  \param bytes  Number of bytes to transfer
  \return 1 if successful, otherwise 0
  */

char twi_write_data(unsigned char* indata, char bytes)
{
    unsigned char index, ack = 0;

    if(!twi_start_cond())
        return 0;
    if(!send_slave_address(WRITE))
        return 0;	

    for(index = 0; index < bytes; index++)
    {
        ack = i2c_write_byte(indata[index]);
        if(!ack)
            break;		
    }
    //put stop here
    write_scl(1);
    __builtin_avr_delay_cycles(SCL_SDA_DELAY);
    write_sda(1);
    return ack;

}


/*! \brief Writes a byte on TWI.
  \param byte Data 
  \return 1 if successful, otherwise 0
  */
char i2c_write_byte(unsigned char byte)
{
    char bit;
    for (bit = 0; bit < 8; bit++) 
    {
        write_sda((byte & 0x80) != 0);
        toggle_scl();//goes high
        __builtin_avr_delay_cycles(DELAY);
        toggle_scl();//goes low
        byte <<= 1;
        __builtin_avr_delay_cycles(DELAY);
    }
    //release SDA
    SET_SDA_IN();
    toggle_scl(); //goes high for the 9th clock
    //Check for acknowledgment
    if(READ_SDA())
    {
        return 0;			
    }
    __builtin_avr_delay_cycles(DELAY);
    //Pull SCL low
    toggle_scl(); //end of byte with acknowledgment. 
    //take SDA
    SET_SDA_OUT();
    __builtin_avr_delay_cycles(DELAY); 
    return 1;

}	
/*! \brief Reads data into buffer.
  \param data Pointer to data buffer
  \param bytes  Number of bytes to read
  \return 1 if successful, otherwise 0
  */
char twi_read_data(unsigned char* data, char bytes)
{
    unsigned char index,success = 0;
    if(!twi_start_cond())
        return 0;
    if(!send_slave_address(READ))
        return 0;	
    for(index = 0; index < bytes; index++)
    {
        success = i2c_read_byte(data, bytes, index);
        if(!success)
            break; 
    }
    //put stop here
    write_scl(1);
    __builtin_avr_delay_cycles(SCL_SDA_DELAY);
    write_sda(1);
    return success;


}	

/*! \brief Reads one byte into buffer.
  \param rcvdata Pointer to data buffer
  \param bytes  Number of bytes to read
  \param index Position of the incoming byte in hte receive buffer 
  \return 1 if successful, otherwise 0
  */
char i2c_read_byte(unsigned char* rcvdata, unsigned char bytes, unsigned char index)
{
    unsigned char byte = 0;
    unsigned char bit = 0;
    //release SDA
    SET_SDA_IN();
    for (bit = 0; bit < 8; bit++) 
    {
        toggle_scl();//goes high
        if(READ_SDA())
            byte|= (1 << (7- bit));
        __builtin_avr_delay_cycles(DELAY);
        toggle_scl();//goes low
        __builtin_avr_delay_cycles(DELAY);
    }
    rcvdata[index] = byte;
    //take SDA
    SET_SDA_OUT();
    if(index < (bytes-1))
    {
        write_sda(0);
        toggle_scl(); //goes high for the 9th clock
        __builtin_avr_delay_cycles(DELAY);
        //Pull SCL low
        toggle_scl(); //end of byte with acknowledgment. 
        //release SDA
        write_sda(1);
        __builtin_avr_delay_cycles(DELAY);
    }
    else //send NACK on the last byte
    {
        write_sda(1);
        toggle_scl(); //goes high for the 9th clock
        __builtin_avr_delay_cycles(DELAY);
        //Pull SCL low
        toggle_scl(); //end of byte with acknowledgment. 
        //release SDA
        __builtin_avr_delay_cycles(DELAY);
    }		
    return 1;

}	
/*! \brief Writes SCL.
  \param x tristates SCL when x = 1, other wise 0
  */
void write_scl (char x)
{
    if(x)
    {
        DDR_SCL &= ~(1 << SCL); //tristate it
        //check clock stretching
        while(!READ_SCL());
    }
    else
    {
        DDR_SCL |= (1 << SCL); //output 
        PORT_SCL &= ~(1 << SCL); //set it low

    }
}

/*! \brief Writes SDA.
  \param x tristates SDA when x = 1, other wise 0
  */
void write_sda (char x)
{
    if(x)
    {
        DDR_SDA &= ~(1 << SDA); //tristate it
    }
    else
    {
        DDR_SDA |= (1 << SDA); //output 
        PORT_SDA &= ~(1 << SDA); //set it low

    }
}

/*! \brief Toggles SCL.
*/
void toggle_scl() 
{
    if(PIN_SCL & (1<<SCL))
    {
        DDR_SCL |= (1 << SCL); //output
        PORT_SCL &= ~(1 << SCL); //set it low
    }
    else
    {
        DDR_SCL &= ~(1 << SCL); //tristate it
        while(!READ_SCL());
    }		
}
