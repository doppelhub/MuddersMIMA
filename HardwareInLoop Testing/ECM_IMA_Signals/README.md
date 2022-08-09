# MuddersMIMA
 Mudders Take on Manual IMA Control in the G1 Honda Insight

This test code simulates the following OEM signals from the ECM:

-CMDPWR
-MAMODE1
-MAMODE2

To use:
-Load this fimware onto a 328p
-Connect the above signals as defined in "#define PIN_..." statements
-Open Serial Monitor
-Type one of the commands below:

'$I' to request idle IMA (no assist or regen)
'$Axxx', where 'xxx' is the percent assist requested
'$Rxxx', where 'xxx' is the percent regen  requested

That's about it