#include "gpiodef.h"
#include "st7920.h"
#include "lcd_fonts.h"
#include <stdio.h>
#include <string.h>

/****************************************
* clone from https://github.com/wumingyu12/STM32-ST7290-LCD12864-20150729/tree/master
****************************************/
#define delay_us swspi_hal_delay_us
#define delay_ms swspi_hal_delay_ms

//void st7920_set_psb(st7920_t *d, uint8_t val) { (void)d; (void)val; } //always low
void st7920_set_rs(st7920_t *d, uint8_t val) { (void)d; (void)val; } //enable, always high
//void st7920_set_sclk(st7920_t *d, uint8_t val) { (void)d; (void)val; }
//void st7920_set_sid(st7920_t *d, uint8_t val) { (void)d; (void)val; }

void st7920_init(st7920_t *d, swspi_t *spi, spi_gpio_t *rs, spi_gpio_t *psb) {
	if(spi) d->pDev = spi; else d->pDev = NULL;
	//if(rs) d->rs = rs; else d->rs = NULL;
	//if(psb) d->psb = psb; else d->psb = NULL;
	//if(rs) { swspi_setgpmode(rs, 1); st7920_set_rs(d, 0); } //LCD_RS_1;  //CSһֱ���ߣ�ʹ��Һ������ֱ�ӽ�VCC��
	//if(psb) { swspi_setgpmode(psb, 1); st7920_set_psb(d, 0); } //LCD_PSB_0; //һֱ���ͣ��ô��ڷ�ʽ���� ����ֱ�ӽӵأ�
	d->CurrentX = 0; d->CurrentY = 0;
	delay_ms(40);
	st7920_cmd(d, LCD_BASIC); //0x30
	delay_ms(10);
	st7920_cmd(d, LCD_CLS); //0x01
	delay_ms(10);
	st7920_cmd(d, LCD_HOME); //0x02
	delay_ms(10);
	st7920_cmd(d, LCD_ADDRINC); //0x06
	delay_ms(10);
	st7920_cmd(d, LCD_DISPLAYON); //0x0c
	delay_ms(10);
	st7920_cmd(d, LCD_EXTEND); //0x34 ͼ�ο���������ʾ��
	delay_ms(10);
	st7920_fill(d, 0x00);    //�������RAM
	delay_ms(10);
	st7920_cmd(d, LCD_GFXMODE); //0x36 ͼ�ο���������ʾ��
	delay_ms(10);
	memset(d->buf, 0, 1024);
}

void st7920_cmd(st7920_t *d, uint8_t Cbyte) {
	uint8_t cmd[4] = { 0xf8, 0xf0, 0xf0, 0};
	cmd[1] &= Cbyte;
	cmd[2] &= Cbyte << 4;
	//st7920_set_rs(d, 1); //LCD_RS_1;
	swspi_write(d->pDev, cmd, 3);
	//st7920_set_rs(d, 0); //LCD_RS_0;
}

void st7920_data(st7920_t *d, uint8_t Dbyte ) {
	uint8_t cmd[4] = { 0xfa, 0xf0, 0xf0, 0};
	cmd[1] &= Dbyte;
	cmd[2] &= Dbyte << 4;
	//st7920_set_rs(d, 1); //LCD_RS_1;
	swspi_write(d->pDev, cmd, 3);
	//st7920_set_rs(d, 0); //LCD_RS_0;
}

void st7920_pdata(st7920_t *d, void *pvBUF, int32_t bytesize) {
	uint8_t cmd = 0xfa;
	//st7920_set_rs(d, 1); //LCD_RS_1;
	swspi_write(d->pDev, &cmd, 1);
	swspi_write(d->pDev, pvBUF, bytesize);
	//st7920_set_rs(d, 0); //LCD_RS_0;
}


void st7920_cursor(st7920_t *d, uint8_t x, uint8_t y) {
	uint8_t k = 0x80; //0x80, 0x90, 0x88, 0x98
	k |= (y & 1) << 4;
	k |= (y & 2) << 2;
	k += x;
	d->CurrentX = x; d->CurrentY = y;
	st7920_cmd(d, k);
}

void st7920_string(st7920_t *d,uint8_t x,uint8_t y,char *s) {
	st7920_cmd(d, 0x30); //�����׼ģʽ
	st7920_cursor(d,x,y);
	while(*s) { st7920_data(d, (uint8_t)*s); s++; }
	st7920_cmd(d, 0x36); //����ͼ��ģʽ
}

void st7920_fill(st7920_t *d, uint8_t color) { //�������RAM
	uint8_t y, z=ST7920_WIDTH/4;
	st7920_cmd(d, 0x34);
	memset(d->buf, color, z);	
	for(y=128; y<160; y++) {
		st7920_cmd(d, y); //row from, 0-63
		st7920_cmd(d, 128); //col from, 0-7, w=128
		st7920_pdata(d, d->buf, z);
	}
	st7920_cmd(d, 0x36);
}

void st7920_gfxmode(st7920_t *d, uint8_t mode) {
	if(mode) { 
		st7920_cmd(d, LCD_EXTEND); 
		st7920_cmd(d, LCD_GFXMODE);
		d->mode = 1;
	} else { 
		st7920_cmd(d, LCD_EXTEND); 
		st7920_cmd(d, LCD_TXTMODE); 
		d->mode = 0;
	}
}

/*	ST7920 had 256(w)x64(h)bit, total 2KiB GDRAM, in 128x64pixel product, 
	GDRAM operation mode is 256x32pixel. Imagination 128x64 is 256x32 in real,
	also row address only accept 0-31, but col address accept 0-15 
	(16bit data per one address, 16x16=256bit), and auto accumulation 
	able from 0 to 15 in the st7920.
	col address bit 4 determined display at y position 0-31 or 32-63.
	for example, row=1,col=1 (cmd 0x81,0x81), display position is 2,2,
	row=1,col=9 (cmd 0x81,0x89),display position is 2,34	
*/ 
void st7920_update(st7920_t *d) {
	int x, y;
	uint32_t *_ps = (uint32_t*)d->buf;
	uint32_t _t[8]; uint8_t *_pt=(uint8_t*)_t;	
	st7920_cmd(d, 0x34);
	for(y=0x80; y<0xa0; y++, _ps+=4) {
		st7920_cmd(d, y); //row from, 0-63
		st7920_cmd(d, 0x80); //col from, 0-7, w=128
		for(x=0; x<4; x++) { //for one row, total 32byte
			_t[2] = (_ps[x] & 0xf0f0f0f0);
			_t[3] = (_ps[x] & 0x0f0f0f0f) << 4;
			_pt[0] = _pt[ 8]; _pt[1] = _pt[12];
			_pt[2] = _pt[ 9]; _pt[3] = _pt[13];
			_pt[4] = _pt[10]; _pt[5] = _pt[14];
			_pt[6] = _pt[11]; _pt[7] = _pt[15];
			st7920_pdata(d, _t, 8);
		}

		for(x=ST7920_WIDTH; x<ST7920_WIDTH+4; x++) { //for one row, total 8byte
			_t[2] = (_ps[x] & 0xf0f0f0f0);
			_t[3] = (_ps[x] & 0x0f0f0f0f) << 4;
			_pt[0] = _pt[ 8]; _pt[1] = _pt[12];
			_pt[2] = _pt[ 9]; _pt[3] = _pt[13];
			_pt[4] = _pt[10]; _pt[5] = _pt[14];
			_pt[6] = _pt[11]; _pt[7] = _pt[15];
			st7920_pdata(d, _t, 8);
		}			
	}
	st7920_cmd(d, 0x36);
}

void st7920_DrawPixel(st7920_t *d, uint8_t x, uint8_t y, uint8_t color) {
    if(x >= ST7920_WIDTH || y >= ST7920_HEIGHT) { return; }
    // Draw in the right color //optimize for 128pixel width.
    if(color) {
        d->buf[(x >> 3) + (y << 4)] |= 1 << ((x&7)^7); 
    } else { 
        d->buf[(x >> 3) + (y << 4)] &= ~(1 << ((x&7)^7));
    }	
}
// 1-31 => 95-125
// 32-126 => 0-94
// 127-255 => 126-254
char st7920_WriteChar2(st7920_t *d, uint8_t ch, FontDef *font, uint8_t color) {
    uint32_t i, j;
    if(ch == 0) return 0;
    if (ST7920_WIDTH < (d->CurrentX + font->FontWidth) ||
        ST7920_HEIGHT < (d->CurrentY + font->FontHeight)) { return 0; }
	//printf("--2--\n");
    if(font->FontHeight <= 8) {
        uint8_t *px, m;
        px = ((uint8_t*)font->data);
	//printf("--3-1--\n");
        if(font->bBigTable == 0) px += (ch - 32) * font->FontWidth;
        else px += (ch - 1) * font->FontWidth;

        for(i = 0; i < font->FontWidth; i++) {
            for(m=0x80,j=0; j<font->FontHeight; j++, m>>=1) {
                st7920_DrawPixel(d, d->CurrentX + i, (d->CurrentY + j), (px[i] & m) ? color : !color);
            }
        }
	} else if(font->FontWidth==8 && font->FontHeight==16) {
        uint8_t *px, m;
        if (ch < 32 || ch > 126) return 0;
        px = (uint8_t*)font->data + ((ch-32)*font->FontHeight);
        for(i = 0; i < font->FontHeight; i++) {
            for(m=0x80,j=0; j<font->FontWidth; j++, m>>=1) {
                st7920_DrawPixel(d, d->CurrentX + j, (d->CurrentY + i), (px[i] & m) ? color : !color);
            }
        }
    } else {
        uint16_t *px, m;
	//printf("--3-2--\n");
        if (ch < 32 || ch > 126) return 0;
        px = (uint16_t*)font->data + ((ch-32)*font->FontHeight);
        for(i = 0; i < font->FontHeight; i++) {
            for(m=0x8000,j=0; j<font->FontWidth; j++, m>>=1) {
                st7920_DrawPixel(d, d->CurrentX + j, (d->CurrentY + i), (px[i] & m) ? color : !color);
            }
        }
    }

    d->CurrentX += font->FontWidth;
    return ch;
}

void st7920_strin2(st7920_t *d, char *s, FontDef *font, uint8_t color) {
	while(*s) { st7920_WriteChar2(d, *s, font, color); s++; }
}
