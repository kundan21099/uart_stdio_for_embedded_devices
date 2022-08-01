/*
 * print.c / print.h
 *
 * Created on: Aug 1, 2022
 * Author: Kundan Vishnu Gaikwad
 *
 * Copyright (c) 2022 Kundan Vishnu Gaikwad
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT, OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>


extern UART_HandleTypeDef huart1;
int uart_1=1;
int uart_2=2;
char rxBuff[100] = {0};


char *_strrev (char *str)
{
  int i;
  int len = 0;
  char c;
  if (!str)
    return NULL;
  while(str[len] != '\0'){
    len++;
  }
  for(i = 0; i < (len/2); i++)
  {
    c = str[i];
    str [i] = str[len - i - 1];
    str[len - i - 1] = c;
  }
  return str;
}

char * _itoa(int i, char *strout, int base)
{
  char *str = strout;
  int digit, sign = 0;
  if (i < 0) {
    sign = 1;
    i *= -1;
  }
  while(i) {
    digit = i % base;
    *str = (digit > 9) ? ('A' + digit - 10) : '0' + digit;
    i = i / base;
    str ++;
  }
  if(sign) {
  *str++ = '-';
  }
  *str = '\0';
  _strrev(strout);
  return strout;
}

int printk(int uartNo,char * str, ...)
{
  va_list vl;
  int i = 0, j=0;
  char pbuff[200]={0}, tmp[20];
  char * str_arg;

  va_start( vl, str );
  while (str && str[i])
  {
    if(str[i] == '%'){
      i++;
      switch (str[i]) {
        case 'c': {
          pbuff[j] = (char)va_arg( vl, int );
          j++;
          break;
        }
        case 'd': {
          _itoa(va_arg( vl, int ), tmp, 10);
          strcpy(&pbuff[j], tmp);
          j += strlen(tmp);
          break;
        }
        case 'x': {
          _itoa(va_arg( vl, int ), tmp, 16);
          strcpy(&pbuff[j], tmp);
          j += strlen(tmp);
          break;
        }
        case 'o': {
          _itoa(va_arg( vl, int ), tmp, 8);
          strcpy(&pbuff[j], tmp);
          j += strlen(tmp);
          break;
        }
        case 's': {
          str_arg = va_arg( vl, char* );
          strcpy(&pbuff[j], str_arg);
          j += strlen(str_arg);
          break;
        }
        case 'b': {
          _itoa(va_arg( vl, int ), tmp, 2);
          strcpy(&pbuff[j], tmp);
          j += strlen(tmp);
          break;
        }
      }
    } else {
      pbuff[j] =str[i];
      j++;
    }
    i++;
  }

  if(uartNo==uart_1){
	  for(int x=0;x<=j;x++){
		  USART1->DR=pbuff[x];
		  while(!(USART1->SR & (1<<6)));
		  pbuff[x]='\0';
	  }
  }else if(uartNo==uart_2){
	  for(int x=0;x<=j;x++){
		  USART2->DR=pbuff[x];
		  while(!(USART2->SR & (1<<6)));
		  pbuff[x]='\0';
	  }
  }

  va_end(vl);
  return j;
}

int scank(int uartNo,char* str, ...)
{

  va_list vl;
  int i = 0, j=0, ret = 0;
  char buff[100] = {0}, tmp[20], c;
  char *out_loc;
  while(c != ""){
	  if(uartNo==uart_1){
		  while(!(USART1->SR & (1<<5)));
		  c=USART1->DR;
		  if(c==13){
			break;
		  }
		  buff[i] = c;
		  USART1->DR=c;
		  while(!(USART1->SR & (1<<6)));
		  i++;
	  }
	  if(uartNo==uart_2){
		  while(!(USART2->SR & (1<<5)));
		  c=USART2->DR;
		  if(c==13){
			  break;
		  }
		  buff[i] = c;
		  USART2->DR=c;
		  while(!(USART2->SR & (1<<6)));
		  i++;
	  }
  }
  va_start( vl, str );
  i = 0;

  while (str && str[i]) {
    if (str[i] == '%') {
      i++;
    switch (str[i]) {
        case 'c': {
          * (char *)va_arg( vl, char* ) = buff[j];
          j++;
          ret++;
          i++;
          break;
        }
        case 'd': {
          * (int *)va_arg( vl, int* ) = \
          strtol(&buff[j], &out_loc, 10);
          j += out_loc -&buff[j];
          ret++;
          i++;
          break;
        }
        case 'x': {
          * (int *)va_arg( vl, int* ) = \
          strtol(&buff[j], &out_loc, 16);
          j += out_loc -&buff[j];
          ret++;
          i++;
          break;
        }
        case 'o': {
          * (int *)va_arg( vl, int* ) = \
          strtol(&buff[j], &out_loc, 8);
          j += out_loc -&buff[j];
          ret++;
          i++;
          break;
        }
        case 's': {
          out_loc = (char *)va_arg( vl, char* );
          strcpy(out_loc, &buff[j]);
          j += strlen(&buff[j]);
          ret++;
          i++;
          break;
        }
        case 'b': {
          * (int *)va_arg( vl, int* ) = \
          strtol(&buff[j], &out_loc, 2);
          j += out_loc -&buff[j];
          ret++;
          i++;
          break;
        }
      }
    } else {
    	if(str[i]==32){
    		i++;
    	}
    	if(str[i]!=37){
    		buff[j] =str[i];
    		j++;
    		i++;
    	}
    }

//    i++;
  }



  va_end(vl);
  return ret;
}


