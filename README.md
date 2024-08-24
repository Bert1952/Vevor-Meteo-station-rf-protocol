cheap weatherstation sold by Vevor.<br/> 
https://nl.aliexpress.com/item/1005005999388306.html?
spm=a2g0o.order_list.order_list_main.5.11d779d25eGC8l&gatewayAdapt=glo2nld<br/>
TX freq 868.35 Mhz FSK. Transmitter IC found CMOSTEC CMT2119A.<br/>
Receiver prob CMT2219A. Not sure about this as pcb in the display was mounted backward<br/>
Used Hope’s rfm22b in direct mode to catch the data.<br/>
Found a datastream starting with a pos/neg pulse 1350us each.<br/>
Next 39 preamble, 90us bittime<br/>
Followed by sync C0AAC0AA .<br/>
the 6 bytes probably product/security codes 14,AA,00,24,0D,1E<br/>
the actual message starts at byte 1. a simple checksum is provided summing byte 1 to 19, and store the result in byte 20<br/>
Total 32 bytes are transmitted starting after the Sync, 6 product/security, 16 data, 10 unknow (discarded)<br/>
Byte 6: HI byte of temperature<br/>
Byte 7 LO byte of temperature. Temperature has a offset of 500<br/>
Byte 8 Humidity 0-100<br/>
Byte 9 HI byte of average (need to be checked), may be 2 of 3 bits are used of this byte.<br/>
Byte 10 LO byte of average windspeed. ouput is km/h with decimal thus km/h *10<br/>
Byte 11 Gust windspeed. Relation to km/u *62/100. Not clear which unit the gust is expressed, but comparing the result with windfinder gives a good result in km/h<br/>
Byte 12 Direction byte bit 8: bit 2 of this byte is setted. Other bits probably batt stat etc<br/>
Byte 13 Direction 0-0ff 8 bits of total 9 (0-360 degrees)<br/>
Byte 14 and 15. 16 bits raincounter. Byte 8 is HI part. each increment will be 0.2 mm<br/>
Byte 16 UV radiation. range 0-16<br/>
Byte 17,18,19 24 bits of Solar radiation. Byte 19 is HI byte, byte 20 middle, byte 21 Low part. divide by 36000 to get w/m2<br/>
Byte 20 checksum over byte 1 to 19.<br/>

Byte 21 Seems to be byte 19 +1. No clue why. see examples F4,CA,F5------42,11,43-------DE,1C,DF etc

Bytes 22-32 unknown. steady data C1,B2,65,A9,74,39,C7,38,C9,1E Might be a firmware signature

examples outputs 14,AA,00,24,0D,1E,02,DA,40,01,06,01,01,57,00,1A,06,98,88,F4,CA,F5,C1,B2,65,9A,74,39,C7,38,C9,1E 

14,AA,00,24,0D,1E,02,DA,41,01,0E,02,01,8D,01,1A,06,98,61,42,11,43,C1,B2,65,9A,74,39,C7,38,C9,1E 

14,AA,00,24,0D,1E,02,D0,41,01,1A,07,01,93,01,1A,05,94,C6,DE,1C,DF,C1,B2,65,9A,74,39,C7,38,C9,1E

14,AA,00,24,0D,1E,02,CC,42,01,26,0A,01,A6,01,1A,05,95,BF,2C,12,2D,C1,B2,65,9A,74,39,C7,38,C9,1E 

14,AA,00,24,0D,1E,02,CD,43,01,2E,0B,01,B5,01,1A,05,95,6E,53,71,54,C1,B2,65,9A,74,39,C7,38,C9,1E 

14,AA,00,24,0D,1E,02,D6,3F,01,0B,01,01,BC,01,1A,06,99,3A,16,E4,17,C1,B2,65,9A,74,39,C7,38,C9,1E

Might be that the processor is running on a low xtal freq. The bitrate is 90us, but a pulse can vary between 60-110 us. Note: the tx message is transmitted twice in 1 message. So will the first fail, the soft can catch the second
Added a watchdog in case the program will crash. It just fits in 8k atmega88a

