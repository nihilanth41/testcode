/*http://raspi.tv/2013/how-to-use-soft-pwm-in-rpi-gpio-pt-2-led-dimming-and-motor-speed-control*/

/* From comment: 
Thank you for this tutorial. It’s very clear and helpful. I will suggest that a more common cycling loop pattern would be:

lo, hi, step = 0, 10, 1
while True:
for i in range(min(lo, hi), max(lo, hi), abs(step)):
#white.ChangeDutyCycle(lo)
#red.ChangeDutyCycle(hi)
print (lo, hi)
lo += step
hi -= step
step *= -1

and if you hate the use of min, max, and abs you can always duplicate the variable at the beginning:

lo, hi, step = 0, 10, 1
rlo, rhi, rstep = lo, hi, step
while True:
for i in range(rlo, rhi, rstep):
#white.ChangeDutyCycle(lo)
#red.ChangeDutyCycle(hi)
print (lo, hi)
lo += step
hi -= step
step *= -1
*/
