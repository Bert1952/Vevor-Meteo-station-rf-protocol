cheap weatherstation sold by Vevor. 

https://nl.aliexpress.com/item/1005005999388306.html?spm=a2g0o.order_list.order_list_main.5.11d779d25eGC8l&gatewayAdapt=glo2nld

TX freq 868.35 Mhz  FSK. Transmitter IC found CMOSTEC CMT2119A. 

Receiver prob CMT2219A. Not sure about this as pcb in the display was mounted backward

Used rfm22b in direct mode to catch the data.

Found a datastream starting with a pos/neg pulse 1350us each.

Next 39 preamble, 90us bittime

Followed by sync C0AAC0AA .

the 6 words probably product/security codes 14,AA,00,24,0D,1E

Total 32 words are transmitted starting after the Sync, 6 product/security, 16 data, 10 unknow

Word 6: HI word of temperature

Word 7 LO word of temperature. Temperature has a offset of 500

Word 8 Humidity 0-100

Word 9 HI word of average (need to be checked), may be 2 of 3 bits are used of this word.

Word 10 LO word of average windspeed. the only relation i could find is by converting this number *100/85 gives km/h

Word 11 Gust windspeed. Relation to km/u  *73/100

Word 12 Direction word bit 8: bit 2 of this word is setted.  Other bits probably  batt stat etc

Word 13 Direction 0-0ff  8 bits of total 9 (0-360 degrees)

Word 14 and 15.  16 bits raincounter. Word 8 is HI part.  each increment will be 0.2 mm

Word 16  UV radiation. range 0-16

Word 17,18,19  24 bits of Solar radiation.  Word 19 is HI word, word 20 middle, word 21 Low part. divide by 36000 to get w/m2 

Word 20 checksum over word 1 to 19.  

Word 21 Seems to be word 19 +1.  No clue why. see examples F4,ca,F5------42,11,43-------DE,1C,DF  etc

Words 22-32 unknown. steady data C1,B2,65,A9,74,39,C7,38,C9,1E Might be a firmware signature

examples outputs
14,AA,00,24,0D,1E,02,DA,40,01,06,01,01,57,00,1A,06,98,88,F4,CA,F5,C1,B2,65,9A,74,39,C7,38,C9,1E
14,AA,00,24,0D,1E,02,DA,41,01,0E,02,01,8D,01,1A,06,98,61,42,11,43,C1,B2,65,9A,74,39,C7,38,C9,1E
14,AA,00,24,0D,1E,02,D0,41,01,1A,07,01,93,01,1A,05,94,C6,DE,1C,DF,C1,B2,65,9A,74,39,C7,38,C9,1E
14,AA,00,24,0D,1E,02,CC,42,01,26,0A,01,A6,01,1A,05,95,BF,2C,12,2D,C1,B2,65,9A,74,39,C7,38,C9,1E
14,AA,00,24,0D,1E,02,CD,43,01,2E,0B,01,B5,01,1A,05,95,6E,53,71,54,C1,B2,65,9A,74,39,C7,38,C9,1E
14,AA,00,24,0D,1E,02,D6,3F,01,0B,01,01,BC,01,1A,06,99,3A,16,E4,17,C1,B2,65,9A,74,39,C7,38,C9,1E

the output signals from the tx are not quite stable. even the update rate which should be 20 seconds can be 20-30 seconds.

Might be that the processor is running on a low xtal freq. The bitrate is 90us, but a pulse can vary between 60-110 us.
Note: the tx message is transmitted twice in 1 message. So will the first fail, the soft can catch the second 
