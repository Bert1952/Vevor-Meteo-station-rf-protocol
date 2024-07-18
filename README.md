cheap weatherstation sold by Vevor. 

https://nl.aliexpress.com/item/1005005999388306.html?spm=a2g0o.order_list.order_list_main.5.11d779d25eGC8l&gatewayAdapt=glo2nld

TX freq 868.35 Mhz  FSK. Transmitter IC found CMOSTEC CMT2119A. 

Receiver prob CMT2219A. Not sure about this as pcb in the display was mounted backward

Used rfm22b in direct mode to catch the data.

Found a datastream starting with a pos/neg pulse 1350us each.

Next 39 preamble, 90us bittime

Followed by sync C0AAC0AA .

the 5 words probably product/security codes 0x14,0AAh,xxx,xxx,xxx

Total 30 words are transmitted starting after the Sync, 5 product/security, 15 data, 10 unknow

Word 0: HI word of temperature

Word 1 LO word of temperature. Temperature has a offset of 500

Word 2 Humidity 0-100

Word 3 HI word of average (need to be checked), may be 2 of 3 bits are used of this word.

Word 4 LO word of average windspeed. the only relation i could find is by converting this number *100/85 gives km/h

Word 5 Gust windspeed. Relation to km/u  *73/100

Word 6 Direction word bit 8: bit 2 of this word is setted.  Other bits probably  batt stat etc

Word 7 Direction 0-0ff  8 bits of total 9 (0-360 degrees)

Word 8 and 9.  16 bits raincounter. Word 8 is HI part.  each increment will be 0.2 mm

Word 10 and 11  UV radiation.  Word 10 is HI part.  
Word 12,13,14  24 bits of Solar radiation.  Word 12 is HI word, word 13 middle, word 14 Low part
Word 15: Checksum.  Need to be investigated how this is composed

Words 16-24 unknown. steady data C1,B2,65,A9,74,39,C7,38,C9,1E Might be a firmware signature

