cheap weatherstation sold by Vevor. 

https://nl.aliexpress.com/item/1005005999388306.html?spm=a2g0o.order_list.order_list_main.5.11d779d25eGC8l&gatewayAdapt=glo2nld
TX freq 868.3 Mhz  FSK. Transmitter IC found CMOSTEC CMT2119A. 
Receiver prob CMT2219A. Not sure about this as pcb in the display was mounted backward
Used rfm22b in direct mode to catch the data.
Found a datastream starting with a pos/neg pulse 1350us each.
Next 39 preamble, 90us bittime
Followed by sync C0AAC0AAB .
the 5 words probably product/security codes 0x14,0AAh,xxx,xxx,xxx
Followed by data
Word 0: HI word of temperature
Word 1 LO word of temperature. Temperature has a offset of 500
Word 2 Humidity 0-100
Word 3 HI word of average (need to be checked)
Word 4 LO word of average windspeed. the only relation i could find is by converting this number *100/85 gives km/h
Word 5 Gust windspeed. Relation to km/u  *73/100
Word 6 Direction word bit 8: bit 2 of this word is setted.  Other bits probably  batt stat etc
