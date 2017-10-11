REM program to calculate wether a number is prime or not
REM in hawk script.
REM written by anup pokhrel
while 1
	print "Enter a number",
	input n%
	p_in=n
	gosub 13
wend

13 isprime=1
for i=2 to p_in\2+1
	if p_in%i=0 then
		print i,"x",p_in/i," = ",p_in
		isprime=0
	endif
next
if isprime then
	print "Prime"
else
	print"Not prime"
endif
return

